#include "hzpch.h"
#include "Reflection.h"
#include "TypeUtils.h"

#pragma region >> field

template <typename Class, typename Field>
FieldInfo Reflection::RegisterField(const std::string& fieldName, uint32_t Offset)
{
	// Type 관리자에 Class, Field 정보를 모두 추가한다. 
	auto registerClass = RegistType<Class>();
	auto registerField = RegistType<Field>();

	bool isArray = TypeUtils::CheckArray<Field>();
	bool isIterable = TypeUtils::CheckIterable<Field>();

	if (isArray)
	{
		isIterable = true;
	}

	return RegisterField(
		GetTypeID<Class>(),
		VariableId::Create<Field>(),
		fieldName,
		Offset,
		sizeof(Field),
		alignof(Field),
		isIterable,
		isArray
	);
}

template<typename Field>
FieldInfo Reflection::RegisterField(TypeId classId, const std::string& fieldName, uint32_t Offset)
{
	auto registerField = RegistType<Field>();

	bool isArray = TypeUtils::CheckArray<Field>();
	bool isIterable = TypeUtils::CheckIterable<Field>();

	if (isArray)
	{
		isIterable = true;
	}

	return RegisterField(
		classId,
		VariableId::Create<Field>(),
		fieldName,
		Offset,
		sizeof(Field),
		alignof(Field),
		isIterable,
		isArray
	);
}


FieldInfo Reflection::RegisterField(TypeId classId, VariableId FieldId, const std::string& fieldName, uint32_t Offset,
	uint32_t Size, uint32_t Align, bool isIterable, bool isArray)
{
	static StaticContainerData& containerData = getStaticContainerData();

	TypeInfo& typeInfo = containerData.typeInfos[classId];

	FieldInfo info{};
	info.m_Name = fieldName;
	info.m_VariableId = FieldId;
	info.m_Offset = Offset;
	info.m_Size = Size;
	info.m_Align = Align;
	info.m_IsIterable = isIterable;
	info.m_IsArray = isArray;

	// array 는 무조건 iterable 이어야 한다.
	if (info.m_IsArray && !info.m_IsIterable)
	{
		assert(false);
	}

	typeInfo.m_fieldInfos.insert(info);

	return info;;
}

size_t Reflection::GetFieldCount(TypeId id)
{
	return size_t();
}

const std::set<FieldInfo>* Reflection::GetFieldInfos(TypeId classId)
{
	static StaticContainerData& containerData = getStaticContainerData();

	if (containerData.typeInfos.find(classId) == containerData.typeInfos.end())
	{
		return nullptr;
	}

	const TypeInfo& typeInfo = containerData.typeInfos[classId];

	return &typeInfo.m_fieldInfos;
}

const FieldInfo* Reflection::GetFieldInfo(TypeId classId, const std::string& FieldName)
{
	static StaticContainerData& containerData = getStaticContainerData();

	if (containerData.typeInfos.find(classId) == containerData.typeInfos.end())
	{
		return nullptr;
	}

	const TypeInfo& typeInfo = containerData.typeInfos[classId];

	for (const auto& field : typeInfo.m_fieldInfos)
	{
		if (field.m_Name == FieldName)
		{
			return& field;
		}
	}

	return nullptr;
}

bool Reflection::IsDerivedClass(TypeId baseType, TypeId childType)
{
	if (baseType == childType)
	{
		return true;
	}

	const TypeInfo* dinfo = GetTypeInfo(childType);

	if (!IsRegistered(baseType)) THROW("Type is not registed");
	if (!IsRegistered(childType)) THROW("Type is not registed");
	if (dinfo == nullptr) THROW("Type is not registed");

	const TypeInfo* baseInfo = GetTypeInfo(baseType);

	bool foundResult = std::find(baseInfo->m_SubTypes.begin(), baseInfo->m_SubTypes.end(), childType) != baseInfo->m_SubTypes.end();

	return foundResult;
}

#pragma endregion

#pragma region >> reflection

std::string Reflection::GetTypeName(const TypeId& id)
{
	const TypeInfo& typeInfo = *GetTypeInfo(id);

	return typeInfo.m_Name;
}

uint64_t Reflection::Hash(std::string_view str)
{
	std::uint64_t hash_value = 0xcbf29ce484222325ULL;
	constexpr std::uint64_t prime = 0x100000001b3ULL;
	for (char c : str)
	{
		hash_value ^= static_cast<std::uint64_t>(c);
		hash_value *= prime;
	}
	return hash_value;
}


Reflection::StaticContainerData& Reflection::getStaticContainerData()
{
	/*
	inline static std::unordered_map<uint64_t, TypeInfo> TypeInfos{};
	위와 같이 하는 것 대신에 StaticData 라는 struct 안에 선언해준 이유 ?
	static 변수이므로, 어떤 시점에 초기화 되는지 컨트롤 할 수 없다.
	하지만 아래와 같이 세팅하면 GetStatics() 를 호출하는 순간 초기화되도록
	할 수 있다.
	현재 Application 의 경우 RegisterTypeId() 안에서 GetStatics() 를 호출한다.
	즉, REgisterTypeID 함수도 static 이다.
	해당 함수가 호출될 때 unordered_map 이 반드시 초기화되어 있어야 한다.
	이러한 순서를 보장하기 위한 장치이다.
	*/
	static StaticContainerData data{};
	return data;
};

Reflection::TypeInfo* Reflection::GetTypeInfo(const TypeId& id)
{
	static StaticContainerData& containerData = getStaticContainerData();

	return &containerData.typeInfos[id];
}

const auto& Reflection::GetAllTypeInformation()
{
	static StaticContainerData& containerData = getStaticContainerData();

	return containerData.typeInfos;
}

bool Reflection::IsRegistered(const TypeId& id)
{
	static StaticContainerData& containerData = getStaticContainerData();
	bool isContainId = containerData.typeInfos.find(id) != containerData.typeInfos.end();
	return isContainId;
}

void Reflection::RegisterBase(TypeId baseType, TypeId subType)
{
	TypeInfo* baseTypeInfo = GetTypeInfo(baseType);

	bool contain = std::find(baseTypeInfo->m_SubTypes.begin(), baseTypeInfo->m_SubTypes.end(), subType) != baseTypeInfo->m_SubTypes.end();

	if (contain) return;

	baseTypeInfo->m_SubTypes.push_back(subType);
}

uint32 Reflection::GetTypeSize(const TypeId& id)
{
	const TypeInfo& typeInfo = *GetTypeInfo(id);

	return typeInfo.m_Size;
}

uint32 Reflection::GetTypeAlignment(TypeId id)
{
	const TypeInfo& typeInfo = *GetTypeInfo(id);

	return typeInfo.m_Align;
}

DataType Reflection::GetDataType(TypeId id)
{
	static StaticContainerData& containerData = getStaticContainerData();
	
	return containerData.typeDataCodeMap[id];
}

bool Reflection::IsFundamental(TypeId id) 
{
	const TypeInfo& typeInfo = *GetTypeInfo(id);

	return typeInfo.m_Flags[TypeFlags_IsFundamental];
}

bool Reflection::IsIntegral(TypeId id) 
{
	const TypeInfo& typeInfo = *GetTypeInfo(id);

	return typeInfo.m_Flags[TypeFlags_IsIntegral];
}

bool Reflection::IsFloatingPoint(TypeId id) 
{
	const TypeInfo& typeInfo = *GetTypeInfo(id);

	return typeInfo.m_Flags[TypeFlags_IsFloatingPoint];
}

bool Reflection::IsTriviallyCopyable(TypeId id) 
{
	const TypeInfo& typeInfo = *GetTypeInfo(id);

	return typeInfo.m_Flags[TypeFlags_IsTriviallyCopyable];
}

#pragma endregion

#pragma region >> reflection_private


#pragma endregion

/*StaticContainerData*/
#pragma region >> static_container

Reflection::StaticContainerData::StaticContainerData()
{
	registDataType<bool>(DataType::BOOL);
	registDataType<char>(DataType::CHAR);
	registDataType<unsigned char>(DataType::CHAR);
	registDataType<int16>(DataType::INT16);
	registDataType<uint16>(DataType::UINT16);
	registDataType<int32>(DataType::INT32);
	registDataType<uint32>(DataType::UINT32);
	registDataType<int64>(DataType::INT64);
	registDataType<uint64>(DataType::UINT64);
	registDataType<float>(DataType::FLOAT);
	registDataType<double>(DataType::DOUBLE);
	registDataType<std::string>(DataType::STRING);
	registDataType<char*>(DataType::STRING);
	registDataType<const char*>(DataType::STRING);
}

template<typename T>
void Reflection::StaticContainerData::registDataType(DataType code)
{
	static const TypeId typeId = Reflection::GetTypeID<T>();
	typeDataCodeMap.insert(std::make_pair(typeId, code));
}

#pragma endregion