#pragma once

#include "hzpch.h"

namespace Hazel
{

struct InputStream;

struct OutputStream;

struct StreamTarget
{
    virtual void Serialize(OutputStream &stream) = 0;

    virtual void Deserialize(InputStream &stream) = 0;
};


class Stream
{
public:
    virtual ~Stream(){};

    virtual int64 GetCurrentPos() = 0;

    virtual void SetPos(int64 pos) = 0;

    virtual void SerializeData(const void *ptr, size_t size) = 0;

    virtual void DeserializeData(void *ptr, size_t size) = 0;

    virtual size_t GetDataLength() const = 0;

    virtual void End() = 0;

    // pointer x, string x
    // sizeof(T) 로 표현할 수 있는 것들
    template <typename T,
              typename std::enable_if<
                  !std::is_pointer<T>::value &&
                  !std::is_same<T, std::string>::value &&
                  !std::is_same<T, std::wstring>::value>::type * = nullptr>
    void Deserialize(T &data)
    {
        constexpr size_t typeSize = sizeof(T);
        DeserializeData((void *)&data, typeSize);
    }

    // 여러 개
    template <typename T,
              typename std::enable_if<
                  !std::is_pointer<T>::value &&
                  !std::is_same<T, std::string>::value &&
                  !std::is_same<T, std::wstring>::value>::type * = nullptr>
    void Deserialize(T &data, size_t count)
    {
        constexpr size_t typeSize = sizeof(T);
        DeserializeData((void *)&data, typeSize * count);
    }

    // pointer x, string x
    // sizeof(T) 로 표현할 수 있는 것들
    template <typename T,
              typename std::enable_if<
                  !std::is_pointer<T>::value &&
                  !std::is_same<T, std::string>::value &&
                  !std::is_same<T, std::wstring>::value>::type * = nullptr>
    void Serialize(const T &t)
    {
        static_assert(!std::is_pointer<T>::value, "Can't write pointer type");
        constexpr size_t typeSize = sizeof(T);
        SerializeData((void *)&t, typeSize);
    }

    template <typename T,
              typename std::enable_if<
                  !std::is_pointer<T>::value &&
                  !std::is_same<T, std::string>::value &&
                  !std::is_same<T, std::wstring>::value>::type * = nullptr>
    void Serialize(const T &t, size_t count)
    {
        static_assert(!std::is_pointer<T>::value, "Can't write pointer type");
        constexpr size_t typeSize = sizeof(T);
        SerializeData((void *)&t, typeSize * count);
    }
};


/*Input Read 만 진행해주는 Stream*/
struct InputStream
{
    Stream *m_Stream;

    explicit InputStream(Stream *stream) : m_Stream(stream){};

    // std::decay : remove ref or qualifier
    // pointer x, Streamable 의 파생 클래스 X
    template <typename T,
              typename std::enable_if<
                  !std::is_pointer<T>::value &&
                  !std::is_base_of<StreamTarget, typename std::decay<T>::type>::
                      value>::type * = nullptr>
    void Deserialize(T &data)
    {
        static_assert(!std::is_same<typename std::decay<T>::type, char>::value,
                      "It's not allow char*");
        static_assert(
            !std::is_same<typename std::decay<T>::type, wchar_t>::value,
            "It's not allow char*");

        m_Stream->Deserialize(data);
    }

    // Streamable 파생 클래스
    template <typename T,
              typename std::enable_if<std::is_base_of<
                  StreamTarget,
                  typename std::decay<T>::type>::value>::type * = nullptr>
    void Deserialize(T &data)
    {
        Deserialize((StreamTarget *)&data);
    }

    // Streamable 파생 클래스 외 + pointer x
    template <
        typename T,
        typename std::enable_if<!std::is_pointer<T>::value>::type * = nullptr>
    void Deserialize(const T &data, size_t count)
    {
        static_assert(!std::is_same<typename std::decay<T>::type, char>::value,
                      "It's not allow char*");
        static_assert(
            !std::is_same<typename std::decay<T>::type, wchar_t>::value,
            "It's not allow wchar*");

        m_Stream->Deserialize(data, count);
    }

    inline void Deserialize(StreamTarget *t)
    {
        t->Deserialize(*this);
    }

    void Deserialize(std::string &data);

    void Deserialize(std::wstring &data);

    template <typename T>
    void Deserialize(std::vector<T> &t)
    {
        static_assert(!std::is_same<typename std::decay<T>::type, char>::value,
                      "It's not allow char*");
        static_assert(
            !std::is_same<typename std::decay<T>::type, wchar_t>::value,
            "It's not allow wchar*");

        size_t len = 0;
        Deserialize(len);
        t.resize(len);
        for (size_t i = 0; i < t.size(); ++i)
        {
            Deserialize(t[i]);
        }
    }

    std::string ReadToEnd();
};

/*Output Write 만 진행해주는 Stream*/
struct OutputStream
{
    Stream *m_Stream;

    explicit OutputStream(Stream *stream) : m_Stream(stream)
    {
    }

    ~OutputStream()
    {
    }

    // pointer X & Streamable type도 아닌 것.
    template <typename T,
              typename std::enable_if<
                  !std::is_pointer<T>::value &&
                  !std::is_base_of<StreamTarget, typename std::decay<T>::type>::
                      value>::type * = nullptr>
    void Serialize(const T &t)
    {
        m_Stream->Serialize(t);
    }

    // Streamable 의 파생 class
    template <typename T,
              typename std::enable_if<std::is_base_of<
                  StreamTarget,
                  typename std::decay<T>::type>::value>::type * = nullptr>
    void Serialize(const T &t)
    {
        Serialize((StreamTarget *)&t);
    }

    template <
        typename T,
        typename std::enable_if<!std::is_pointer<T>::value>::type * = nullptr>
    void Serialize(const T &t, uint64 count)
    {
        m_Stream->Serialize(t);
    }

    inline void Serialize(StreamTarget *t)
    {
        t->Serialize(*this);
    }

    void Serialize(const char *str);

    void Serialize(const wchar_t *str);

    void Serialize(const std::string &t);

    void Serialize(const std::wstring &t);

    void WriteText(const char *str);

    template <typename T>
    void Serialize(const std::vector<T> &t)
    {
        Serialize(t.length());
        for (size_t i = 0; i < t.length(); ++i)
        {
            Serialize(t[i]);
        }
    }
};

} // namespace Hazel