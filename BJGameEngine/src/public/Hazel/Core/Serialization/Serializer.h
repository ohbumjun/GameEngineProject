#pragma once

#include "../Reflection/Reflection.h"
#include "hzpch.h"
#include <glm/glm.hpp>

namespace Hazel
{

class BaseObject;
class BaseComponent;
class SerializeTarget;

enum class SerializeType
{
    Serialize,
    Deserialize
};

/*
struct StaticType
{
	static const TypeId BoolType;
	static const TypeId CharType;
	static const TypeId Int8Type;
	static const TypeId Int16Type;
	static const TypeId Int32Type;
	static const TypeId Int64Type;
	static const TypeId Uint8Type;
	static const TypeId Uint16Type;
	static const TypeId Uint32Type;
	static const TypeId Uint64Type;
	static const TypeId DoubleType;
	static const TypeId FloatType;
	static const TypeId StringType;
	static const TypeId VoidPtrType;
};
*/

class Serializer
{
public:
    Serializer(SerializeType type);
    Serializer() = default;
    virtual ~Serializer();

#pragma region SAVE
public:
    // map
    void BeginSaveMap();
    void BeginSaveMap(const TypeId type, void *target = nullptr);
    template <typename T>
    void BeginSaveMap(T *target);
    void EndSaveMap();
    // key
    void SaveKey(const char *key);
    // array
    void BeginSaveSeq(uint64 arrayLength);
    void BeginSaveSeq(TypeId type, uint64 arrayLength);
    template <typename T>
    void BeginSaveSeq(uint64 arrayLength);
    void EndSaveSeq();
    template <
        typename T,
        typename std::enable_if<!std::is_pointer<T>::value>::type * = nullptr>
    void Save(const T &data)
    {
        onSave(data);
    }
    void Save(const TypeId type, void *data);
    void Save(const char *key, void *data);
    void Save(const char *key, const SerializeTarget *data);
    void Save(const char *key, const bool data);
    void Save(const char *key, const char data);
    void Save(const char *key, const int8 data);
    void Save(const char *key, const uint8 data);
    void Save(const char *key, const int16 data);
    void Save(const char *key, const uint16 data);
    void Save(const char *key, const int32 data);
    void Save(const char *key, const uint32 data);
    void Save(const char *key, const int64 data);
    void Save(const char *key, const uint64 data);
    void Save(const char *key, const glm::vec2 &data);
    void Save(const char *key, const glm::vec3 &data);
    void Save(const char *key, const glm::vec4 &data);
    void Save(const char *key, const glm::mat4 &data);
    void Save(const char *key, const float data);
    void Save(const char *key, const double data);
    void Save(const char *key, const std::string &data);
    void Save(const char *key, const char *data);
    void SaveRaw(const char *key, void *buffer, size_t size);

    template <typename T>
    void Save(const char *key, std::list<T> &t)
    {
        static TypeId type = Reflection::RegistType<T>();

        onSaveKey(key);
        onBeginSaveSeq(type, t.size());
        for (size_t i = 0; i < t.size(); ++i)
        {
            onSave((T &)t[i]);
        }
        onEndSaveSeq();
    }
    template <typename T>
    void Save(const char *key, std::vector<T> &t)
    {
        static TypeId type = Reflection::RegistType<T>();

        onSaveKey(key);
        onBeginSaveSeq(type, t.size());
        for (size_t i = 0; i < t.size(); ++i)
        {
            onSave((T &)t[i]);
        }
        onEndSaveSeq();
    }

protected:
    virtual void onBeginSaveMap() = 0;
    virtual void onBeginSaveMap(TypeId type) = 0;
    virtual void onSaveKey(const char *key) = 0;
    virtual void onSave(const bool data) = 0;
    virtual void onSave(const char data) = 0;
    virtual void onSave(const int8 data) = 0;
    virtual void onSave(const uint8 data) = 0;
    virtual void onSave(const int16 data) = 0;
    virtual void onSave(const uint16 data) = 0;
    virtual void onSave(const int32 data) = 0;
    virtual void onSave(const uint32 data) = 0;
    virtual void onSave(const int64 data) = 0;
    virtual void onSave(const uint64 data) = 0;
    virtual void onSave(const glm::vec2 &data) = 0;
    virtual void onSave(const glm::vec3 &data) = 0;
    virtual void onSave(const glm::vec4 &data) = 0;
    virtual void onSave(const glm::mat4 &data) = 0;
    virtual void onSave(const float data) = 0;
    virtual void onSave(const double data) = 0;
    virtual void onSave(const std::string &data) = 0;
    virtual void onSave(const char *data) = 0;
    virtual void onSaveRaw(void *buffer, size_t size) = 0;
    virtual void onBeginSaveSeq(uint64 arrayLength) = 0;
    virtual void onBeginSaveSeq(TypeId type, uint64 arrayLength) = 0;
    virtual void onEndSaveSeq() = 0;
    virtual void onEndSaveMap() = 0;

    // origin : writeRef
    virtual void onSaveLink(const BaseObject *data);
    virtual void onSaveLink(const BaseComponent *data);

#pragma endregion

#pragma region READ
public:
    // object
    void BeginLoadMap();
    void BeginLoadMap(TypeId type, void *target);
    template <typename T>
    void BeginLoadMap(T *target)
    {
        static TypeId typeId = Reflection::GetTypeID<T>();
        BeginLoadMap(typeId, target);
    }

    // key
    void LoadKey(const char *key);

    // seq
    size_t BeginLoadSeq();
    size_t BeginLoadSeq(TypeId type);
    size_t BeginLoadSeq(TypeId type, void *target);
    template <typename T>
    size_t BeginLoadSeq()
    {
        return BeginLoadSeq(Reflection::GetTypeID<T>());
    }
    void EndLoadSeq();

    void Load(const TypeId type, void *data);
    void Load(const char *key, void *data);
    void Load(const char *key, const TypeId type, void *data);
    void Load(const char *key, SerializeTarget *data);
    void Load(const char *key, bool &data);
    void Load(const char *key, char &data);
    void Load(const char *key, int8 &data);
    void Load(const char *key, uint8 &data);
    void Load(const char *key, int16 &data);
    void Load(const char *key, uint16 &data);
    void Load(const char *key, int32 &data);
    void Load(const char *key, uint32 &data);
    void Load(const char *key, int64 &data);
    void Load(const char *key, uint64 &data);
    void Load(const char *key, glm::vec2 &data);
    void Load(const char *key, glm::vec3 &data);
    void Load(const char *key, glm::vec4 &data);
    void Load(const char *key, glm::mat4 &data);
    void Load(const char *key, float &data);
    void Load(const char *key, double &data);
    void Load(const char *key, std::string &data);

    template <typename T>
    void Load(T &data)
    {
        onLoad(data);
        m_Record.recentKey = "";
    }

    template <typename T>
    void Load(const char *key, std::vector<T> &t)
    {
        static TypeId type = Reflection::RegistType<T>();

        LoadKey(key);

        size_t len = BeginLoadSeq(type);
        t.resize(len);

        for (size_t i = 0; i < len; ++i)
        {
            Load(t[i]);
        }

        EndLoadSeq();
    }

    void LoadBuffer(const char *key, void *buffer, size_t size);
    void LoadBuffer(void *buffer, size_t size);
    void EndLoadMap();


    /*read*/
    virtual void onBeginLoadMap() = 0;
    virtual void onBeginLoadMap(TypeId type) = 0;
    virtual void onEndLoadMap() = 0;

    virtual void onLoadKey(const char *key) = 0;
    virtual void onLoad(bool &data) = 0;
    virtual void onLoad(char &data) = 0;
    virtual void onLoad(int8 &data) = 0;
    virtual void onLoad(uint8 &data) = 0;
    virtual void onLoad(int16 &data) = 0;
    virtual void onLoad(uint16 &data) = 0;
    virtual void onLoad(int32 &data) = 0;
    virtual void onLoad(uint32 &data) = 0;
    virtual void onLoad(int64 &data) = 0;
    virtual void onLoad(uint64 &data) = 0;
    virtual void onLoad(glm::vec2 &data) = 0;
    virtual void onLoad(glm::vec3 &data) = 0;
    virtual void onLoad(glm::vec4 &data) = 0;
    virtual void onLoad(glm::mat4 &data) = 0;
    virtual void onLoad(float &data) = 0;
    virtual void onLoad(double &data) = 0;
    virtual void onLoad(std::string &data) = 0;
    virtual void onLoad(char *data)
    {
    }
    virtual void onLoadBuffer(void *buffer, size_t size) = 0;

    virtual size_t onBeginLoadSeq() = 0;
    virtual size_t onBeginLoadSeq(TypeId type) = 0;
    virtual void onEndLoadSeq() = 0;

    virtual void onLoadLink(BaseObject *&object);
    virtual void onLoadLink(BaseComponent *&data);

#pragma endregion

    // void Serializer::WStartObject(const TypeId type, void* target)
    // 해당 함수에서 값이 채워진다.
    // 여기서 채워지는 index, property 의 사용용도는 무엇이지 ?
    struct RecordData
    {
        std::string key = nullptr;

        // 나를 만든 순간에서의 Recor.datas 상의 idx ?
        size_t recordIndex = -1;

        TypeId m_Type = 0;

        void *m_ValuePointer = nullptr;

        RecordData(TypeId type,
                   void *ptr,
                   const std::string &key = "",
                   int32 index = -1);
    };

    struct Record
    {
        std::string recentKey = "";
        std::vector<RecordData> datas;

        void Init()
        {
            recentKey = "";
            datas.clear();
        }

    } m_Record;

protected:
    // basic type 들을 등록하는 함수
    static void initCustomDefaultTypes();

protected:
    SerializeType m_SerializeType;
    int32 m_Version = -1;
    static std::set<TypeId> m_CustomDefaultTypes;
};

} // namespace Hazel