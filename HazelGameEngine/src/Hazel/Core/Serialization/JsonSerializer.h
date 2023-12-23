#pragma once

#include "Serializer.h"
#include "JsonSerializer.h"

class JsonSerializer : public Serializer
{
public:
	JsonSerializer();
	JsonSerializer(const char* json);
	virtual ~JsonSerializer();

	std::string GetFinalResult();

private:
	virtual void onBeginSaveMap() override;
	virtual void onBeginSaveMap(TypeId type) override;
	virtual void onSaveKey(const char* key) override;
	virtual void onSave(const bool data) override;
	virtual void onSave(const char data) override;
	virtual void onSave(const int8 data) override;
	virtual void onSave(const uint8 data) override;
	virtual void onSave(const int16 data) override;
	virtual void onSave(const uint16 data) override;
	virtual void onSave(const int32 data) override;
	virtual void onSave(const uint32 data) override;
	virtual void onSave(const int64 data) override;
	virtual void onSave(const uint64 data) override;
	virtual void onSave(const glm::vec2& data)  override;
	virtual void onSave(const glm::vec3& data)  override;
	virtual void onSave(const glm::vec4& data)  override;
	virtual void onSave(const glm::mat4& mat4)  override;
	virtual void onSave(const float data) override;
	virtual void onSave(const double data) override;
	virtual void onSave(const std::string& data) override;
	virtual void onSave(const char* data) override;
	virtual void onSaveRaw(void* buffer, size_t size) override;
	virtual void onBeginSaveSeq(uint64 arrayLength);
	virtual void onBeginSaveSeq(TypeId type, uint64 arrayLength);
	virtual void onEndSaveSeq();
	virtual void onEndSaveMap();

private:
	virtual void onBeginLoadMap() override;
	virtual void onBeginLoadMap(TypeId type) override;
	virtual void onEndLoadMap() override;

	virtual void onLoadKey(const char* key) override;
	virtual void onLoad(bool& data) override;
	virtual void onLoad(char& data) override;
	virtual void onLoad(int8& data) override;
	virtual void onLoad(uint8& data) override;
	virtual void onLoad(int16& data) override;
	virtual void onLoad(uint16& data) override;
	virtual void onLoad(int32& data) override;
	virtual void onLoad(uint32& data) override;
	virtual void onLoad(int64& data) override;
	virtual void onLoad(uint64& data) override;
	virtual void onLoad(glm::vec2& data) override;
	virtual void onLoad(glm::vec3& data) override;
	virtual void onLoad(glm::vec4& data) override;
	virtual void onLoad(glm::mat4& data) override;
	virtual void onLoad(float& data) override;
	virtual void onLoad(double& data) override;
	virtual void onLoad(std::string& data) override;
	virtual void onLoad(char* data) override;
	virtual void onLoadBuffer(void* buffer, size_t size) override;

	virtual size_t onBeginLoadSeq() override;
	virtual size_t onBeginLoadSeq(TypeId type) override;
	virtual void onEndLoadSeq() override;
private:
	void init(const char* json);

	struct JsonRecord
	{
		void* value = nullptr;

		// Record 가 Array 일 경우 해당 숫자를 증가시켜가면서 array 
		// 내의 원소를 읽어간다.
		size_t m_ArrayElemNum = 0; 

		JsonRecord() = default;
		JsonRecord(void* value) : value(value) {}
	};

	void* getValue(JsonRecord& context);

private : // Write
	void* m_StringBuffer = nullptr;
	void* m_JsonWriter	= nullptr;
	uint m_KeyCnt	= 0;

private : // Read
	void* m_Document = nullptr;
	std::vector<JsonRecord> m_ReadRecord;
	std::stack<const char*> m_KeyRecord;
};

