#include "hzpch.h"
#include "Hazel/Core/Serialization/Serializer.h"
#include "Hazel/Core/Serialization/SerializeTarget.h"

std::set<TypeId> Serializer::m_CustomDefaultTypes;

void Serializer::initCustomDefaultTypes()
{
	static bool init = false;

	if (init)
	{
		return;
	}

	init = true;

	// m_DefaultTypes.insert(Reflection::RegistType<vec2>());
}

Serializer::RecordData::RecordData(TypeId type, void* ptr, const std::string& property, int32 index)
	: m_Type(type)
	, m_ValuePointer(ptr)
	, key(property)
	, recordIndex(index)
{
};

Serializer::Serializer(SerializeType type)
	: m_SerializeType(type)
{
	initCustomDefaultTypes();
	m_Record.Init();
};

Serializer::~Serializer()
{
};

#pragma region SAVE

void Serializer::BeginSaveMap()
{
	m_Record.datas.emplace_back(0, nullptr);
	onBeginSaveMap();
}

void Serializer::BeginSaveMap(const TypeId type, void* target)
{
	m_Record.datas.emplace_back(type, target, m_Record.recentKey, m_Record.datas.size());
	onBeginSaveMap(type);
}

template<typename T>
void Serializer::BeginSaveMap(T* target)
{
	BeginSaveMap(Reflection::GetTypeID<T>(), target);
}

void Serializer::EndSaveMap()
{
	if (m_Record.datas.empty() == false)
	{
		m_Record.datas.pop_back();
	}
	onEndSaveMap();
}

void Serializer::SaveKey(const char* key)
{
	m_Record.recentKey = key;
	onSaveKey(key);
}

void Serializer::BeginSaveSeq(uint64 arrayLength)
{
	onBeginSaveSeq(arrayLength);
}

void Serializer::BeginSaveSeq(TypeId type, uint64 arrayLength)
{
	onBeginSaveSeq(type, arrayLength);
}

template<typename T>
void Serializer::BeginSaveSeq(uint64 arrayLength)
{
	BeginSaveSeq(Reflection::GetTypeID<T>(), arrayLength);
}

void Serializer::EndSaveSeq()
{
	onEndSaveSeq();
}

void Serializer::Save(const TypeId type, void* data)
{
	const auto info = Reflection::GetTypeInfo(type);

	void* pointer = data;

	DataType code = Reflection::GetDataType(type);
	
	if (type == Reflection::GetTypeID<bool>())
	{
		bool* v = static_cast<bool*>(pointer);
		onSave(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<char>())
	{
		char* v = static_cast<char*>(pointer);
		onSave(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<int16>())
	{
		int16* v = static_cast<int16*>(pointer);
		onSave(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<int32>())
	{
		int32* v = static_cast<int32*>(pointer);
		onSave(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<int64>())
	{
		int64* v = static_cast<int64*>(pointer);
		onSave(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<uint8>())
	{
		uint8* v = static_cast<uint8*>(pointer);
		onSave(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<uint16>())
	{
		uint16* v = static_cast<uint16*>(pointer);
		onSave(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<uint32>())
	{
		uint32* v = static_cast<uint32*>(pointer);
		onSave(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<uint64>())
	{
		uint64* v = static_cast<uint64*>(pointer);
		onSave(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<float>())
	{
		float* v = static_cast<float*>(pointer);
		onSave(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<double>())
	{
		double* v = static_cast<double*>(pointer);
		onSave(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<std::string>())
	{
		static TypeId StdStringType = Reflection::GetTypeID<std::string>();
		std::string* v = static_cast<std::string*>(pointer);
		Save(*v);
		return;
	}
	else
	{
		if (m_CustomDefaultTypes.find(type) != m_CustomDefaultTypes.end())
		{
			// 여기에 걸린다면 custon type 이라는 의미..?
		}
		else
		{
			// 실제 Object pointer 형태를 seriailze 하는 것이다.
			if (Reflection::IsDerivedClass(Reflection::GetTypeID<BaseObject>(), info->m_OriginalType)
				&& info->m_PointerCount == 1)
			{
				const BaseObject* obj = nullptr;
				obj = pointer ? *static_cast<BaseObject**>(pointer) : nullptr;
				onBeginSaveMap();
				onSaveLink(obj);
				onEndSaveMap();
				return;
			}
		}
	}

	THROW("Not support type");
}

void Serializer::Save(const char* key, const SerializeTarget* data )
{
	SaveKey(key);
	const_cast<SerializeTarget&>(*data).Serialize(this);
}

void Serializer::Save(const char* key, const bool data)
{
	SaveKey(key);
	onSave(data);
}

void Serializer::Save(const char* key, const char data)
{
	SaveKey(key);
	onSave(data);
}

void Serializer::Save(const char* key, const int8 data)
{
	SaveKey(key);
	onSave(data);
}

void Serializer::Save(const char* key, const uint8 data)
{
	SaveKey(key);
	onSave(data);
}

void Serializer::Save(const char* key, const int16 data)
{
	SaveKey(key);
	onSave(data);
}

void Serializer::Save(const char* key, const uint16 data)
{
	SaveKey(key);
	onSave(data);
}

void Serializer::Save(const char* key, const int32 data)
{
	SaveKey(key);
	onSave(data);
}

void Serializer::Save(const char* key, const uint32 data)
{
	SaveKey(key);
	onSave(data);
}

void Serializer::Save(const char* key, const int64 data)
{
	SaveKey(key);
	onSave(data);
}

void Serializer::Save(const char* key, const uint64 data)
{
	SaveKey(key);
	onSave(data);
}

void Serializer::Save(const char* key, const glm::vec2& data)
{
	SaveKey(key);
	onSave(data);
}

void Serializer::Save(const char* key, const glm::vec3& data)
{
	SaveKey(key);
	onSave(data);
}

void Serializer::Save(const char* key, const glm::vec4& data)
{
	SaveKey(key);
	onSave(data);
}

void Serializer::Save(const char* key, const glm::mat4& data)
{
	SaveKey(key);
	onSave(data);
}

void Serializer::Save(const char* key, const float data)
{
	SaveKey(key);
	onSave(data);
}

void Serializer::Save(const char* key, const double data)
{
	SaveKey(key);
	onSave(data);
}

void Serializer::Save(const char* key, const std::string& data)
{
	SaveKey(key);
	onSave(data);
}

void Serializer::Save(const char* key, const char* data)
{
	SaveKey(key);
	onSave(data);
}

void Serializer::SaveRaw(const char* key, void* buffer, size_t size)
{
	SaveKey(key);
	onSaveRaw(buffer, size);
}

#pragma endregion

#pragma region READ

void Serializer::BeginLoadMap()
{
	m_Record.datas.emplace_back(0, nullptr, m_Record.recentKey);
	m_Record.recentKey = "";
	onBeginLoadMap();
}

void Serializer::BeginLoadMap(TypeId type, void* target)
{
	m_Record.datas.emplace_back(type, target, m_Record.recentKey);
	m_Record.recentKey = "";
	onBeginLoadMap();
}

void Serializer::onEndLoadMap()
{
	if (m_Record.datas.empty() == false)
	{
		m_Record.datas.pop_back();
	}

	onEndLoadMap();

	// 이 코드가 왜 있는 거지 ?
	// int accessIndex = m_History.acceses.size() - 1;
	// 
	// // if (!m_History.acceses.empty() && _archiveContext.acceses.Back().index > -1)
	// if (!m_History.acceses.empty() && m_History.acceses[accessIndex].index > -1)
	// {
	// 	m_History.acceses[accessIndex].index++;
	// }
}

void Serializer::LoadKey(const char* key)
{
	m_Record.recentKey = key;
	onLoadKey(key);
}

size_t Serializer::BeginLoadSeq()
{
	m_Record.datas.emplace_back(0, nullptr, m_Record.recentKey);
	m_Record.recentKey = "";
	return onBeginLoadSeq();
}

size_t Serializer::BeginLoadSeq(TypeId type)
{
	m_Record.datas.emplace_back(type, nullptr, m_Record.recentKey, 0);
	m_Record.recentKey = nullptr;
	return onBeginLoadSeq(type);
}

size_t Serializer::BeginLoadSeq(TypeId type, void* target)
{
	m_Record.datas.emplace_back(type, target, m_Record.recentKey, 0);
	m_Record.recentKey = nullptr;
	return onBeginLoadSeq(type);
}

void Serializer::EndLoadSeq()
{
	if (m_Record.datas.empty() == false)
	{
		m_Record.datas.pop_back();
	}

	onEndLoadSeq();

	// 아래 로직은 왜 필요한 거지 ?
	// int accessIndex = m_History.acceses.size() - 1;
	// 
	// if (!m_History.acceses.empty() && m_History.acceses[accessIndex].index > -1)
	// {
	// 	m_History.acceses[accessIndex].index++;
	// }
}

void Serializer::Load(const TypeId type, void* data)
{
	const auto info = Reflection::GetTypeInfo(type);

	void* pointer = data;

	DataType code = Reflection::GetDataType(type);

	if (type == Reflection::GetTypeID<bool>())
	{
		bool* v = static_cast<bool*>(pointer);
		onLoad(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<char>())
	{
		char* v = static_cast<char*>(pointer);
		onLoad(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<int8>())
	{
		int8* v = static_cast<int8*>(pointer);
		onLoad(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<int16>())
	{
		int16* v = static_cast<int16*>(pointer);
		onLoad(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<int32>())
	{
		int32* v = static_cast<int32*>(pointer);
		onLoad(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<int64>())
	{
		int64* v = static_cast<int64*>(pointer);
		onLoad(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<uint8>())
	{
		uint8* v = static_cast<uint8*>(pointer);
		onLoad(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<uint16>())
	{
		uint16* v = static_cast<uint16*>(pointer);
		onLoad(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<uint32>())
	{
		uint32* v = static_cast<uint32*>(pointer);
		onSave(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<uint64>())
	{
		uint64* v = static_cast<uint64*>(pointer);
		onLoad(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<float>())
	{
		float* v = static_cast<float*>(pointer);
		onLoad(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<double>())
	{
		double* v = static_cast<double*>(pointer);
		onLoad(*v);
		return;
	}
	else if (type == Reflection::GetTypeID<std::string>())
	{
		static TypeId StdStringType = Reflection::GetTypeID<std::string>();
		std::string* v = static_cast<std::string*>(pointer);
		Load(*v);
		return;
	}
	else
	{
		if (m_CustomDefaultTypes.find(type) != m_CustomDefaultTypes.end())
		{
			// 여기에 걸린다면 custon type 이라는 의미..?
		}
		else
		{
			// 실제 Object pointer 형태를 seriailze 하는 것이다.
			if (Reflection::IsDerivedClass(Reflection::GetTypeID<BaseObject>(), info->m_OriginalType)
				&& info->m_PointerCount == 1)
			{
				BaseObject* obj = nullptr;
				obj = pointer ? *static_cast<BaseObject**>(pointer) : nullptr;
				onBeginLoadMap();
				onLoadLink(obj);
				onEndLoadMap();
				return;
			}
			else
			{
				THROW("Not support type");
			}
		}
	}
	m_Record.recentKey = "";
}

void Serializer::Load(const char* key, void* data)
{
}

void Serializer::Load(const char* key, const TypeId type, void* data)
{
}

void Serializer::Load(const char* key, SerializeTarget* data)
{
	LoadKey(key);
	(*data).Deserialize(this);
	m_Record.recentKey = "";
}

void Serializer::Load(const char* key, bool& data)
{
	LoadKey(key);
	onLoad(data);
	m_Record.recentKey = "";
}

void Serializer::Load(const char* key, char& data)
{
	LoadKey(key);
	onLoad(data);
	m_Record.recentKey = "";
}

void Serializer::Load(const char* key, int8& data)
{
	LoadKey(key);
	onLoad(data);
	m_Record.recentKey = "";
}

void Serializer::Load(const char* key, uint8& data)
{
	LoadKey(key);
	onLoad(data);
	m_Record.recentKey = "";
}

void Serializer::Load(const char* key, int16& data)
{
	LoadKey(key);
	onLoad(data);
	m_Record.recentKey = "";
}

void Serializer::Load(const char* key, uint16& data)
{
	LoadKey(key);
	onLoad(data);
	m_Record.recentKey = "";
}

void Serializer::Load(const char* key, int32& data)
{
	LoadKey(key);
	onLoad(data);
	m_Record.recentKey = "";
}

void Serializer::Load(const char* key, uint32& data)
{
	LoadKey(key);
	onLoad(data);
	m_Record.recentKey = "";
}

void Serializer::Load(const char* key, int64& data)
{
	LoadKey(key);
	onLoad(data);
	m_Record.recentKey = "";
}

void Serializer::Load(const char* key, uint64& data)
{
	LoadKey(key);
	onLoad(data);
	m_Record.recentKey = "";
}

void Serializer::Load(const char* key, glm::vec2& data)
{
	LoadKey(key);
	onLoad(data);
	m_Record.recentKey = "";
}

void Serializer::Load(const char* key, glm::vec3& data)
{
	LoadKey(key);
	onLoad(data);
	m_Record.recentKey = "";
}

void Serializer::Load(const char* key, glm::vec4& data)
{
	LoadKey(key);
	onLoad(data);
	m_Record.recentKey = "";
}

void Serializer::Load(const char* key, glm::mat4& data)
{
	LoadKey(key);
	onLoad(data);
	m_Record.recentKey = "";
}

void Serializer::Load(const char* key, float& data)
{
	LoadKey(key);
	onLoad(data);
	m_Record.recentKey = "";
}

void Serializer::Load(const char* key, double& data)
{
	LoadKey(key);
	onLoad(data);
	m_Record.recentKey = "";
}

void Serializer::Load(const char* key, std::string& data)
{
	LoadKey(key);
	onLoad(data);
	m_Record.recentKey = "";
}

void Serializer::LoadBuffer(const char* key, void* buffer, size_t size)
{
	LoadKey(key);
	onLoadBuffer(buffer, size);
	m_Record.recentKey = "";
}

void Serializer::LoadBuffer(void* buffer, size_t size)
{
	onLoadBuffer(buffer, size);
}

void Serializer::EndLoadMap()
{
	if (m_Record.datas.empty() == false)
	{
		m_Record.datas.pop_back();
	}

	onEndLoadSeq();

	int lastRecordIdx = m_Record.datas.size() - 1;

	if (m_Record.datas.empty() == false && m_Record.datas[lastRecordIdx].recordIndex > -1)
	{
		m_Record.datas[lastRecordIdx].recordIndex += 1;
	}

}

#pragma endregion

#pragma region private_write

void Serializer::onSaveLink(const BaseObject* data)
{
}

void Serializer::onSaveLink(const BaseComponent* data)
{
	// bool isNullptr = nullptr == data;
	// 
	// Write("isNullptr", isNullptr);
	// if (!isNullptr)
	// {
	// 	LvObject* from = findFromObject();
	// 
	// 	LV_CHECK(nullptr != from, "LvArchive::writeReference error : can't find from object");
	// 
	// 	LvList<uint64> address = LvObjectAddress::MakeRelative(from, data);
	// 	Write("address", address);
	// }
}

#pragma endregion

#pragma region private_read

void Serializer::onLoadLink(BaseObject*& object)
{
	// bool isNullptr = true;
	// Load("isNullptr", isNullptr);
	// object = nullptr;
	// 
	// if (false == isNullptr)
	// {
	// 	uint32 instanceID;
	// }
	
}

void Serializer::onLoadLink(BaseComponent*& data)
{
}
