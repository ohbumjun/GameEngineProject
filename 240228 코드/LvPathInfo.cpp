#include "LvPrecompiled.h"
#include "editor/project/LvPathInfo.h"

#include "LvFileManager.h"
#include "LvFileTable.h"
#include "editor/access/LvSerializeAccess.h"
#include "system/LvFileSystem.h"

#include "editor/private/project/LvFileInfo.h"
#include "editor/asset/LvAsset.h"
#include "editor/asset/LvAssetExtension.h"
#include "editor/asset/LvAssetDatabase.h"
#include "editor/property/LvSerializeHelper.h"
#include "editor/scene/LvEditorSceneManager.h"
#include "engine/component/LvComponentContainable.h"

using namespace Lv;
using namespace Lv::Engine;
using namespace Lv::Editor::Project;

LV_NS_EDITOR_BEGIN

LvAsset* LvPathInfo::FindAsset(const LvHostAccess& rootHost)
{
	const LvComponent* comp= rootHost.GetComponent();
	if (nullptr != comp)
	{
		const LvComponentContainer* container = comp->GetContainer();

		// @donghun 컨테이너가 없는 ComponentHandle은 정상적이지 않으며 해당 이슈 재현 시 이슈 처리 필요
		LV_CHECK(nullptr != container, "Wrong handle");

		return LvAssetDatabase::GetAssetByInstanceId(container->host->GetInstanceId());
	}

#if defined(_DEBUG)
	LvTypeInfo* info = LvReflection::GetTypeInfo(rootHost.GetType());
#endif

	static LvTypeId assetType = LvReflection::GetTypeId<LvAsset>();
	if (LvReflection::IsAssignableFrom(assetType, rootHost.GetType()))
	{
		return static_cast<LvAsset*>(rootHost.GetPointer());
	}

	// @donghun Host는 항상 종단이 Asset까지 연결되어 있어야 함
	LV_LOG(warning, "Not found asset");
	return nullptr;
}

LvPathInfo::LvPathInfo()
	: _baseTable(nullptr)
{
}

LvPathInfo::LvPathInfo(const char* relativePath, const uint32 localId, const Project::LvFileTable* baseTable)
	: _relativePath(relativePath)
	, _localId(localId)
	, _baseTable(baseTable)
{
	applyPath(_relativePath.c_str());
}

LvPathInfo::LvPathInfo(const LvFileInfo& fileInfo, const Project::LvFileTable* baseTable)
	: LvPathInfo(fileInfo.path.c_str(), 0, baseTable)
{}

LvPathInfo::LvPathInfo(const LvAsset* asset)
{
	*this = asset;
}

LvPathInfo::LvPathInfo(const LvHostAccess& rootHost)
	: LvPathInfo(FindAsset(rootHost))
{}

LvPathInfo::LvPathInfo(const LvSerializeProperty& property)
	: LvPathInfo(FindAsset(*(property.GetRootHost())))
{}

LvPathInfo::LvPathInfo(const LvComponentContainable& containable)
	: LvPathInfo(LvAssetDatabase::GetAssetByInstanceId(containable.GetInstanceId()))
{}

LvPathInfo::LvPathInfo(const LvPathInfo& o)
{
	this->_relativePath = o._relativePath;
	this->_name = o._name;
	this->_nameWithoutExtension = o._nameWithoutExtension;
	this->_extension = o._extension;
	this->_localId = o._localId;
	this->_baseTable = o._baseTable;
}

LvPathInfo::LvPathInfo(LvPathInfo&& o)
{
	this->_relativePath = std::move(o._relativePath);
	this->_name = std::move(o._name);
	this->_nameWithoutExtension = std::move(o._nameWithoutExtension);
	this->_extension = std::move(o._extension);
	this->_localId = o._localId;
	this->_baseTable = o._baseTable;

	o._localId = 0;
	o._baseTable = nullptr;
}

void LvPathInfo::Clear()
{
	*this = LvPathInfo{};
}

LvPathInfo& LvPathInfo::operator=(const LvPathInfo& o)
{
	if (this->GetBaseTable() != o.GetBaseTable())
	{
		LV_LOG(warning, "Wrong table");
	}

	_relativePath = o._relativePath;
	_localId = o._localId;

	applyPath(_relativePath.c_str());

	return *this;
}

LvPathInfo& LvPathInfo::operator=(LvPathInfo&& o)
{
	if (this->GetBaseTable() != o.GetBaseTable())
	{
		LV_LOG(warning, "Wrong table");
	}

	this->_relativePath = std::move(o._relativePath);
	this->_name = std::move(o._name);
	this->_nameWithoutExtension = std::move(o._nameWithoutExtension);
	this->_extension = std::move(o._extension);
	this->_localId = o._localId;
	return *this;
}

LvPathInfo& LvPathInfo::operator=(const LvAsset* asset)
{
	if (nullptr == asset)
	{
		Clear();
	}
	else
	{
		_localId = asset->uuid.GetLocalId();

		LvString assetPath;
		if (0 != _localId)
		{
			const LvAsset* mainAsset = LvAssetDatabase::GetAssetByGuid(asset->uuid.GetGuid());
			assetPath = mainAsset->path;
		}
		else
		{
			assetPath = asset->path;
		}

		// @donghun asset의 포인터는 유효하나 경로가 잘못되었음.
		LV_CHECK(!assetPath.IsEmpty(), "wrong path");

		_relativePath = LvFileInfo::ToRelativePathAtResourcesRelativePath(assetPath.c_str());
		applyPath(_relativePath.c_str());
	}
	return *this;
}

bool LvPathInfo::operator==(const LvPathInfo& o) const
{
	if (this->GetBaseTable() != o.GetBaseTable())
	{
		LV_LOG(warning, "Wrong table");
	}

	if (o._relativePath.IsEmpty() || _relativePath.IsEmpty())
	{
		if (o._relativePath.IsEmpty() && _relativePath.IsEmpty()) return true;
		if (!o._relativePath.IsEmpty() && !_relativePath.IsEmpty()) return true;
		return false;
	}
	
	return 0 == strcmp(o._relativePath.c_str(), _relativePath.c_str()) && o._localId == _localId;
}

bool LvPathInfo::operator==(const LvAsset* o) const
{
	if (nullptr != o)
	{
		if (0 == strcmp(LvFileInfo::ToRelativePathAtResourcesRelativePath(o->path.c_str()).c_str(), _relativePath.c_str()))
		{
			if (o->uuid.GetLocalId() == _localId)
			{
				return true;
			}
		}
	}
	return false;
}

void LvPathInfo::Serialize(LvArchive& archive)
{
	LV_CHECK(nullptr == _baseTable, "Not support base table"); // @donghun 현재 다른 베이스 테이블을 Serialize를 지원하지 않음, 필요 시 구현 필요

	archive.WriteStartObject(LvReflection::GetTypeId<LvPathInfo>(), this);
	archive.Write("_relativePath", _relativePath);
	archive.Write("_localId", _localId);
	archive.WriteEndObject();
}

void LvPathInfo::Deserialize(LvArchive& archive)
{
	archive.ReadStartObject(LvReflection::GetTypeId<LvPathInfo>(), this);
	archive.Read("_relativePath", _relativePath);
	archive.Read("_localId", _localId);
	archive.ReadEndObject();

	applyPath(_relativePath.c_str());
}

bool LvPathInfo::IsValid() const
{ 
	return !GetAbsolutePath().IsEmpty();
}

LvAsset* LvPathInfo::GetAsset() const
{
	const LvString relativePathWithoutResources = LvFileInfo::ToResourcesRelativePathAtRelativePath(_relativePath.c_str());
	LvAsset* asset = LvAssetDatabase::GetAssetByPath(LvFileInfo::ToRelativePathAtResourcesRelativePath(relativePathWithoutResources));
	if (0 != _localId && asset != nullptr) asset = asset->GetSubAssetByLocalId(_localId);
	return asset;
}

LvAssetType LvPathInfo::GetAssetType() const
{
	if (GetAsset() != nullptr)
	{
		return GetAsset()->type;
	}
	
	return LvAssetExtension::GetAssetTypeByExtension(lv_path_extension(_relativePath.c_str()));
}

LvString LvPathInfo::GetAbsolutePath() const
{
	return GetBaseTable()->ToAbsolutePath(_relativePath.c_str());
}

const LvFileTable* LvPathInfo::GetBaseTable() const
{
	if(nullptr == _baseTable)
	{
		return LvFileManager::GetTable();
	}
	return _baseTable;
}

void LvPathInfo::applyPath(const char* relativePath)
{
	_relativePath = lv_path_normalize(relativePath);

	if(_relativePath.Length() && _relativePath[0] == LV_DIRECTORY_SEPARATOR_CHAR)
	{
		_relativePath = _relativePath.SubString(1, _relativePath.Length() - 1);
	}

	_name = lv_path_name(_relativePath.c_str());
	_nameWithoutExtension = lv_path_without_extension(_name.c_str());

	// @donghun 확장자가 없는 경우 lv_path_api가 전체경로를 반환하기 때문에 선체크
	if (_name.Contains("."))
	{
		_extension = lv_path_extension_to_lower(_name.c_str());
	}
	else
	{
		_extension.Clear();
	}
}

LV_NS_EDITOR_END
