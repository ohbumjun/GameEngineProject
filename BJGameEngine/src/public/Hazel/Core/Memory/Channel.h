#pragma once

#include "hzpch.h"

class BaseChannel
{
public:
    virtual ~BaseChannel(){};

    virtual int64 GetCurrentPos() = 0;
    virtual void SetPos(int64 pos) = 0;
    virtual void SerializeData(const void *ptr, size_t size) = 0;
    virtual void DeserializeData(void *ptr, size_t size) = 0;
    virtual size_t GetDataLength() const = 0;
    virtual void CloseChannel() = 0;

    template <typename T>
    void Deserialize(T &data, size_t count = 1);

    template <typename T>
    void Serialize(const T &t, size_t count = 1);

protected:
    size_t m_Position = 0;
    size_t m_Length = 0;
};

template <typename T>
void BaseChannel::Deserialize(T &data, size_t count)
{
    constexpr bool isPointer = std::is_pointer<T>::value;
    constexpr bool isString = std::is_same<T, std::string>::value ||
                              std::is_same<T, std::wstring>::value;
    static_assert(isPointer == false && isString == false, "wrong data");

    constexpr size_t typeSize = sizeof(T);
    DeserializeData((void *)&data, typeSize * count);
}

template <typename T>
void BaseChannel::Serialize(const T &t, size_t count)
{
    constexpr bool isPointer = std::is_pointer<T>::value;
    constexpr bool isString = std::is_same<T, std::string>::value ||
                              std::is_same<T, std::wstring>::value;
    static_assert(isPointer == false && isString == false, "wrong data");

    static_assert(!std::is_pointer<T>::value, "Can't write pointer type");
    constexpr size_t typeSize = sizeof(T);
    SerializeData((void *)&t, typeSize * count);
}