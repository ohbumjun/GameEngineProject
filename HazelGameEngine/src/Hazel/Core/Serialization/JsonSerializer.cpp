#include  "hzpch.h"
#include "JsonSerializer.h"
#include "document.h"
#include "writer.h"
#include "Reader.h"
#include "prettywriter.h"
#include "Hazel/Utils/Encoder.h"

using namespace rapidjson;

using JsonWriter= rapidjson::PrettyWriter<rapidjson::StringBuffer>;

JsonSerializer::JsonSerializer() :
	Serializer(SerializeType::Serialize)
{
	m_StringBuffer = new rapidjson::StringBuffer();

	rapidjson::StringBuffer* sbWriter = (rapidjson::StringBuffer*)m_StringBuffer;

	m_JsonWriter = new JsonWriter(*sbWriter);

	static_cast<JsonWriter*>(m_JsonWriter)->SetFormatOptions(rapidjson::kFormatSingleLineArray);
}

JsonSerializer::JsonSerializer(const char* json) :
	Serializer(SerializeType::Deserialize)
{
	init(json);
}

JsonSerializer::~JsonSerializer()
{

	if (m_StringBuffer)
	{
		delete m_StringBuffer;
	}

	if (m_JsonWriter)
	{
		delete m_JsonWriter;
	}

	Document* doc = reinterpret_cast<Document*>(m_Document);

	if (doc)
	{
		delete doc;
	}
}

std::string JsonSerializer::GetFinalResult()
{
	rapidjson::StringBuffer* StringBuffer = static_cast<rapidjson::StringBuffer*>(m_StringBuffer);
	return StringBuffer->GetString();
}

void JsonSerializer::init(const char* json)
{

	Document* doc = new Document;
	doc = new Document;

	// Parsing JSON string into a rapidjson::Value
	doc->Parse(json);

	if (doc->HasParseError())
	{
		assert(false);
	}

	m_Document = doc;
}

#pragma region WRITE

void JsonSerializer::onBeginSaveMap()
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;
	writer->StartObject();
}

void JsonSerializer::onBeginSaveMap(TypeId type)
{
	onBeginSaveMap();
}

void JsonSerializer::onSaveKey(const char* key)
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;
	writer->Key(key);
}

void JsonSerializer::onSave(const bool data)
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;
	writer->Bool(data);
}

void JsonSerializer::onSave(const char data)
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;
	writer->Int(data);
}

void JsonSerializer::onSave(const int8 data)
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;
	writer->Int(data);
}

void JsonSerializer::onSave(const uint8 data)
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;
	writer->Uint(data);
}

void JsonSerializer::onSave(const int16 data)
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;
	writer->Int(data);
}

void JsonSerializer::onSave(const uint16 data)
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;
	writer->Uint(data);
}

void JsonSerializer::onSave(const int32 data)
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;
	writer->Int(data);
}

void JsonSerializer::onSave(const uint32 data)
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;
	writer->Uint(data);
}

void JsonSerializer::onSave(const int64 data)
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;
	writer->Int64(data);
}

void JsonSerializer::onSave(const uint64 data)
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;
	writer->Uint64(data);
}

void JsonSerializer::onSave(const glm::vec2& data)
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;

	writer->StartArray();
	for (int i = 0; i < 2; ++i)
	{
		writer->Double(data[i]);
	}
	writer->EndArray();
}

void JsonSerializer::onSave(const glm::vec3& data)
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;

	writer->StartArray();
	for (int i = 0; i < 3; ++i)
	{
		writer->Double(data[i]);
	}
	writer->EndArray();
}

void JsonSerializer::onSave(const glm::vec4& data)
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;

	writer->StartArray();

	for (int i = 0; i < 4; ++i)
	{
		writer->Double(data[i]);
	}

	writer->EndArray();
}

void JsonSerializer::onSave(const glm::mat4& data)
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;

	writer->StartArray();

	for (int r = 0; r < 4; ++r)
	{
		for (int c = 0; c < 4; ++c)
		{
			writer->Double(data[r][c]);
		}
	}

	writer->EndArray();
}

void JsonSerializer::onSave(const float data)
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;
	writer->Double(data);
}

void JsonSerializer::onSave(const double data)
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;
	writer->Double(data);
}

void JsonSerializer::onSave(const std::string& data)
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;
	writer->String(data.c_str());
}

void JsonSerializer::onSave(const char* data)
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;
	writer->String(data);
}

void JsonSerializer::onSaveRaw(void* buffer, size_t size)
{
	char* src = (char*)buffer;
	int originSize = (int)size;

	// char 개수를 계산한다.
	int encodeSize = Utils::Encoder::base64_encode_length(src, originSize) + 1;

	char* encodeResult = (char*)malloc(encodeSize);

	// void* 데이터로 부터 char 정보를 읽어낸다.
	Utils::Encoder::base64_encode(src, originSize, &encodeResult);

	// encodeResult : byte data 를 char 형태로 변환
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;

	writer->String(encodeResult);

	free(encodeResult);
}

void JsonSerializer::onBeginSaveSeq(uint64 arrayLength)
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;
	writer->StartArray();
}

void JsonSerializer::onBeginSaveSeq(TypeId type, uint64 arrayLength)
{
	onBeginSaveSeq(arrayLength);
}

void JsonSerializer::onEndSaveSeq()
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;
	writer->EndArray();
}

void JsonSerializer::onEndSaveMap()
{
	JsonWriter* writer = (JsonWriter*)m_JsonWriter;
	writer->EndObject();
}


#pragma endregion 

#pragma region READ

void JsonSerializer::onBeginLoadMap()
{
	Value* v = nullptr;

	// 맨 처음 시작
	if (m_ReadRecord.size() == 0)
	{
		v = static_cast<Document*>(m_Document);

		m_ReadRecord.push_back(JsonRecord(v));

		return;
	}

	JsonRecord& prevRecord = m_ReadRecord.back();

	Value* val = static_cast<Value*>(getValue(prevRecord));

	if (val == nullptr || val->IsObject() == false)
	{
		return;
	}

	v = val;

	m_ReadRecord.push_back(JsonRecord(v));
}

void JsonSerializer::onBeginLoadMap(TypeId type)
{
	onBeginLoadMap();
}

void JsonSerializer::onEndLoadMap()
{
	if (m_ReadRecord.empty())
	{
		assert(false);
		return;
	}

	m_ReadRecord.pop_back();
}

void JsonSerializer::onLoadKey(const char* key)
{
	if (m_ReadRecord.size() == 0)
	{
		return;
	}

	//부모의 Value가 nullptr라면 keyStack관련 처리를 다 무시
	JsonRecord& prevRecord = m_ReadRecord.back();

	if (prevRecord.value == nullptr)
	{
		// 적어도 onLoadKey 이전에 onBeginLoadMap 이
		// 실행되었어야 했다.
		return;
	}

	m_KeyRecord.push(key);
}

void JsonSerializer::onLoad(bool& data)
{
	rapidjson::Value* val = nullptr;

	if (m_ReadRecord.empty())
	{
		return;
	}

	//부모 얻어옮
	JsonRecord& prevRecord = m_ReadRecord.back();

	//get next value
	val = static_cast<rapidjson::Value*>(getValue(prevRecord));

	//set Data
	if (nullptr == val || val->IsBool() == false)
	{
		return;
	}

	data = val->GetBool();
}

void JsonSerializer::onLoad(char& data)
{
	rapidjson::Value* val = nullptr;

	if (m_ReadRecord.empty())
	{
		return;
	}

	//부모 얻어옮
	JsonRecord& prevRecord = m_ReadRecord.back();

	//get next value
	val = static_cast<rapidjson::Value*>(getValue(prevRecord));

	//set Data
	if (nullptr == val || val->IsInt() == false)
	{
		return;
	}

	data = val->GetInt();
}

void JsonSerializer::onLoad(int8& data)
{
	rapidjson::Value* val = nullptr;

	if (m_ReadRecord.empty())
	{
		return;
	}

	//부모 얻어옮
	JsonRecord& prevRecord = m_ReadRecord.back();

	//get next value
	val = static_cast<rapidjson::Value*>(getValue(prevRecord));

	//set Data
	if (nullptr == val || val->IsInt() == false)
	{
		return;
	}

	data = val->GetInt();
}

void JsonSerializer::onLoad(uint8& data)
{
	rapidjson::Value* val = nullptr;

	if (m_ReadRecord.empty())
	{
		return;
	}

	//부모 얻어옮
	JsonRecord& prevRecord = m_ReadRecord.back();

	//get next value
	val = static_cast<rapidjson::Value*>(getValue(prevRecord));

	//set Data
	if (nullptr == val || val->IsUint() == false)
	{
		return;
	}

	data = val->GetUint();
}

void JsonSerializer::onLoad(int16& data)
{
	rapidjson::Value* val = nullptr;

	if (m_ReadRecord.empty())
	{
		return;
	}

	//부모 얻어옮
	JsonRecord& prevRecord = m_ReadRecord.back();

	//get next value
	val = static_cast<rapidjson::Value*>(getValue(prevRecord));

	//set Data
	if (nullptr == val || val->IsInt() == false)
	{
		return;
	}

	data = val->GetInt();
}

void JsonSerializer::onLoad(uint16& data)
{
	rapidjson::Value* val = nullptr;

	if (m_ReadRecord.empty())
	{
		return;
	}

	//부모 얻어옮
	JsonRecord& prevRecord = m_ReadRecord.back();

	//get next value
	val = static_cast<rapidjson::Value*>(getValue(prevRecord));

	//set Data
	if (nullptr == val || val->IsUint() == false)
	{
		return;
	}

	data = val->GetUint();
}

void JsonSerializer::onLoad(int32& data)
{
	rapidjson::Value* val = nullptr;

	if (m_ReadRecord.empty())
	{
		return;
	}

	//부모 얻어옮
	JsonRecord& prevRecord = m_ReadRecord.back();

	//get next value
	val = static_cast<rapidjson::Value*>(getValue(prevRecord));

	//set Data
	if (nullptr == val || val->IsInt() == false)
	{
		return;
	}

	data = val->GetInt();
}

void JsonSerializer::onLoad(uint32& data)
{
	rapidjson::Value* val = nullptr;

	if (m_ReadRecord.empty())
	{
		return;
	}

	//부모 얻어옮
	JsonRecord& prevRecord = m_ReadRecord.back();

	//get next value
	val = static_cast<rapidjson::Value*>(getValue(prevRecord));

	//set Data
	if (nullptr == val || val->IsUint() == false)
	{
		return;
	}

	data = val->GetUint();
}

void JsonSerializer::onLoad(int64& data)
{
	rapidjson::Value* val = nullptr;

	if (m_ReadRecord.empty())
	{
		return;
	}

	//부모 얻어옮
	JsonRecord& prevRecord = m_ReadRecord.back();

	//get next value
	val = static_cast<rapidjson::Value*>(getValue(prevRecord));

	//set Data
	if (nullptr == val || val->IsInt64() == false)
	{
		return;
	}

	data = val->GetInt();
}

void JsonSerializer::onLoad(uint64& data)
{
	rapidjson::Value* val = nullptr;

	if (m_ReadRecord.empty())
	{
		return;
	}

	//부모 얻어옮
	JsonRecord& prevRecord = m_ReadRecord.back();

	//get next value
	val = static_cast<rapidjson::Value*>(getValue(prevRecord));

	//set Data
	if (nullptr == val || val->IsUint64() == false)
	{
		return;
	}

	data = val->GetUint64();
}

void JsonSerializer::onLoad(glm::vec2& data)
{
	glm::vec2 readData;

	//부모 얻어옴
	JsonRecord& prevRecord = m_ReadRecord.back();

	// '[' 와 같은 array 의 시작 데이터를 읽는다.
	rapidjson::Value* arrayValue = static_cast<rapidjson::Value*>(getValue(prevRecord));

	if ((nullptr != arrayValue && arrayValue->IsArray()) == false)
	{
		assert(false);
	}

	if (m_ReadRecord.empty())
	{
		return;
	}

	m_ReadRecord.push_back(JsonRecord(arrayValue));

	JsonRecord& prevArrayRecord = m_ReadRecord.back();

	for (int i = 0; i < 2; ++i)
	{
		//get next value
		Value* val = nullptr;
		Value* prevValue = static_cast<Value*>(arrayValue);

		// Array 상에서 특정 index 에 있는 값을 가져온다.
		val = &prevValue->operator[](static_cast<SizeType>(prevArrayRecord.m_ArrayElemNum++));

		//set Data
		if (nullptr == val || val->IsFloat() == false)
		{
			return;
		}

		readData[i] = val->GetFloat();
	}

	data = readData;

	m_ReadRecord.pop_back();
}

void JsonSerializer::onLoad(glm::vec3& data)
{
	glm::vec3 readData;

	//부모 얻어옴
	JsonRecord& prevRecord = m_ReadRecord.back();

	// '[' 와 같은 array 의 시작 데이터를 읽는다.
	rapidjson::Value* arrayValue = static_cast<rapidjson::Value*>(getValue(prevRecord));

	if ((nullptr != arrayValue && arrayValue->IsArray()) == false)
	{
		assert(false);
	}

	if (m_ReadRecord.empty())
	{
		return;
	}

	m_ReadRecord.push_back(JsonRecord(arrayValue));

	JsonRecord& prevArrayRecord = m_ReadRecord.back();

	for (int i = 0; i < 3; ++i)
	{
		//get next value
		Value* val = nullptr;
		Value* prevValue = static_cast<Value*>(arrayValue);

		// Array 상에서 특정 index 에 있는 값을 가져온다.
		val = &prevValue->operator[](static_cast<SizeType>(prevArrayRecord.m_ArrayElemNum++));

		//set Data
		if (nullptr == val || val->IsFloat() == false)
		{
			assert(false);
			return;
		}

		readData[i] = val->GetFloat();
	}

	data = readData;

	m_ReadRecord.pop_back();
}

void JsonSerializer::onLoad(glm::vec4& data)
{
	glm::vec4 readData;

	JsonRecord& prevRecord = m_ReadRecord.back();

	// '[' 와 같은 array 의 시작 데이터를 읽는다.
	rapidjson::Value* arrayValue = static_cast<rapidjson::Value*>(getValue(prevRecord));

	if ((nullptr != arrayValue && arrayValue->IsArray()) == false)
	{
		assert(false);
	}

	if (m_ReadRecord.empty())
	{
		return;
	}

	m_ReadRecord.push_back(JsonRecord(arrayValue));

	JsonRecord& prevArrayRecord = m_ReadRecord.back();

	for (int i = 0; i < 4; ++i)
	{
		//get next value
		Value* val = nullptr;
		Value* prevValue = static_cast<Value*>(arrayValue);

		// Array 상에서 특정 index 에 있는 값을 가져온다.
		val = &prevValue->operator[](static_cast<SizeType>(prevArrayRecord.m_ArrayElemNum++));

		//set Data
		if (nullptr == val || val->IsFloat() == false)
		{
			//  assert(false);
			return;
		}

		readData[i] = val->GetFloat();
	}

	data = readData;

	m_ReadRecord.pop_back();
}

void JsonSerializer::onLoad(glm::mat4& data)
{
	glm::mat4 readData;

	rapidjson::Value* val = nullptr;

	if (m_ReadRecord.empty())
	{
		return;
	}

	//부모 얻어옮
	JsonRecord& prevRecord = m_ReadRecord.back();

	for (int r = 0; r < 4; ++r)
	{
		for (int c = 0; c < 4; ++c)
		{
			//get next value
			val = static_cast<rapidjson::Value*>(getValue(prevRecord));

			//set Data
			if (nullptr == val || val->IsFloat() == false)
			{
				// assert(false);
				return;
			}

			// data = val->GetFloat();
			readData[r][c] = val->GetFloat();
		}
	}

	data = readData;

	m_ReadRecord.pop_back();
}

void JsonSerializer::onLoad(float& data)
{
	rapidjson::Value* val = nullptr;

	if (m_ReadRecord.empty())
	{
		return;
	}

	//부모 얻어옮
	JsonRecord& prevRecord = m_ReadRecord.back();

	//get next value
	val = static_cast<rapidjson::Value*>(getValue(prevRecord));

	//set Data
	if (nullptr == val || val->IsFloat() == false)
	{
		return;
	}

	data = val->GetFloat();
}

void JsonSerializer::onLoad(double& data)
{
	rapidjson::Value* val = nullptr;

	if (m_ReadRecord.empty())
	{
		return;
	}

	//부모 얻어옮
	JsonRecord& prevRecord = m_ReadRecord.back();

	//get next value
	val = static_cast<rapidjson::Value*>(getValue(prevRecord));

	//set Data
	if (nullptr == val || val->IsDouble() == false)
	{
		return;
	}

	data = val->GetDouble();
}

void JsonSerializer::onLoad(std::string& data)
{
	rapidjson::Value* val = nullptr;

	if (m_ReadRecord.empty())
	{
		return;
	}

	//부모 얻어옮
	JsonRecord& prevRecord = m_ReadRecord.back();

	//get next value
	val = static_cast<rapidjson::Value*>(getValue(prevRecord));

	//set Data
	if (nullptr == val || val->IsString() == false)
	{
		return;
	}

	data = val->GetString();
}

void JsonSerializer::onLoad(char* data)
{
	rapidjson::Value* val = nullptr;

	if (m_ReadRecord.empty())
	{
		return;
	}

	//부모 얻어옮
	JsonRecord& History = m_ReadRecord.back();

	//get next value
	val = static_cast<rapidjson::Value*>(getValue(History));

	if (nullptr == val || val->IsString() == false)
	{
		return;
	}

	const char* str = val->GetString();
	strcpy_s(data, sizeof(char), str);
}

void JsonSerializer::onLoadBuffer(void* buffer, size_t size)
{
	char* dest = static_cast<char*>(buffer);
	int originSize = static_cast<int>(size);
	char* result = nullptr;

	// converting binary data into a string of ASCII characters using the Base64 encoding scheme.
	int encodeSize = Utils::Encoder::base64_encode(dest, originSize, &result);

	std::string encodeData;
	onLoad(encodeData);

	if (encodeData.empty())
	{
		assert(false);
		return;
	}

	// string -> buffer
	Utils::Encoder::base64_decode(const_cast<char*>(encodeData.c_str()), encodeSize, dest);
}

size_t JsonSerializer::onBeginLoadSeq()
{
	size_t size = 0;

	rapidjson::Value* v = nullptr;

	if (m_ReadRecord.empty())
	{
		// 시작 document
		v = static_cast<Document*>(m_Document);

		m_ReadRecord.push_back(JsonRecord(v));

		assert(false);

		return 0;
	}

	//부모 얻어옴
	JsonRecord& prevRecord = m_ReadRecord.back();

	//get next value
	rapidjson::Value* val = static_cast<rapidjson::Value*>(getValue(prevRecord));

	if (nullptr != val && val->IsArray())
	{
		//값이 유효하다면 set
		v = val;
		size = v->Size();
	}

	m_ReadRecord.push_back(JsonRecord(v));

	return size;
}

size_t JsonSerializer::onBeginLoadSeq(TypeId type)
{
	return onBeginLoadSeq();
}

void JsonSerializer::onEndLoadSeq()
{
	if (m_ReadRecord.empty())
	{
		assert(false);
		return;
	}

	m_ReadRecord.pop_back();
}

void* JsonSerializer::getValue(JsonRecord& prevRecord)
{ 
	/*
	rapidjson::Value is a versatile class that represents a JSON value.
	JSON values can be of various types, including objects, arrays, strings, numbers, booleans, and null.
	*/
	Value* v = nullptr;
	Value* prevValue = static_cast<Value*>(prevRecord.value);

	bool isPrevArray = false;
	bool isPrevObject = false;
	const char* key = nullptr;

	// Array Type
	if (nullptr != prevValue && prevValue->IsArray())
	{
		isPrevArray = true;

		// Array 상에서 특정 index 에 있는 값을 가져온다.
		v = &prevValue->operator[](static_cast<SizeType>(prevRecord.m_ArrayElemNum++));
	}

	// Object Type
	else if (nullptr != prevRecord.value && prevValue->IsObject())
	{
		isPrevObject = true;

		// Object 에 대응되는 Key 값을 가져온다.
		key = m_KeyRecord.top();

		m_KeyRecord.pop();

		if (prevValue->HasMember(key))
		{
			v = &prevValue->operator[](Value(key, static_cast<SizeType>(strlen(key))));
		}
	}

	if (v == nullptr || (v != nullptr && v->IsNull()))
	{
		// assert(false);
		// 1) array 는 어떤 케이스에서 여기에 걸릴까
		
		// 2) object 의 경우, key 가 없는 녀석이면 여기에 걸릴 수 있다.
		Hazel::Log::GetClientLogger()->warn("No Key : {0}", key);
	}

	return v;
}

#pragma endregion 

