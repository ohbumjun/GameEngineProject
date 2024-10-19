#include "hzpch.h"
#include "Hazel/Core/Serialization/Stream.h"
#include "Hazel/Utils/StringUtil.h"

namespace Hazel
{
void Hazel::InputStream::Deserialize(std::string &data)
{
    uint32 len = 0;

    // string �� ���� ������ ���� �о���δ�.
    m_Stream->Deserialize(len);

    if (len > 1)
    {
        data.reserve(len);
        data.resize(len - 1);
        m_Stream->DeserializeData((void *)data.c_str(), len);
    }
}
void Hazel::InputStream::Deserialize(std::wstring &data)
{
    uint32 len = 0;

    // string �� ���� ������ ���� �о���δ�.
    m_Stream->Deserialize(len);

    if (len > 1)
    {
        data.reserve(len);
        data.resize(len - 1);
        m_Stream->DeserializeData((void *)data.c_str(), len * sizeof(wchar_t));
    }
}
std::string Hazel::InputStream::ReadToEnd()
{
    uint32 len = static_cast<uint32>(m_Stream->GetDataLength());

    if (len > 0)
    {
        std::string r;
        r.resize(len);

        // Stream �� Length ��ŭ m_Stream �� m_Buffer �� ���� �о���δ�.
        m_Stream->DeserializeData((void *)r.c_str(), len);

        return r;
    }

    return std::string();
}
void OutputStream::Serialize(const char *str)
{
    // strlen : ���� \n �� ������ ���̸� �����Ѵ�.
    uint32 len = static_cast<uint32>(Utils::pr_strlen(str) + 1);

    if (len > 1)
    {
        m_Stream->Serialize(len);
        m_Stream->SerializeData((void *)str, len);
    }
    else
    {
        // zero length
        len = 0;

        // length �������� ������´�.
        m_Stream->Serialize(len);
    }
}
void OutputStream::Serialize(const wchar_t *str)
{
    // strlen : ���� \n �� ������ ���̸� �����Ѵ�.
    uint32 len = static_cast<uint32>(Utils::pr_strlen(str) + 1);

    if (len > 1)
    {
        m_Stream->Serialize(len);
        m_Stream->SerializeData((void *)str, len * sizeof(wchar_t));
    }
    else
    {
        // zero length
        len = 0;

        // length �������� ������´�.
        m_Stream->Serialize(len);
    }
}
void OutputStream::Serialize(const std::string &t)
{
    // strlen : ���� \n �� ������ ���̸� �����Ѵ�.
    uint32 len = static_cast<uint32>(t.length() + 1);

    if (len > 1)
    {
        m_Stream->Serialize(len);
        m_Stream->SerializeData((void *)*t.c_str(), len);
    }
    else
    {
        // zero length
        len = 0;

        // length �������� ������´�.
        m_Stream->Serialize(len);
    }
}
void OutputStream::Serialize(const std::wstring &t)
{
    uint32 len = static_cast<uint32>(t.length() + 1);

    if (len > 1)
    {
        m_Stream->Serialize(len);
        m_Stream->SerializeData((void *)*t.c_str(), len);
    }
    else
    {
        // zero length
        len = 0;

        // length �������� ������´�.
        m_Stream->Serialize(len);
    }
}
void OutputStream::WriteText(const char *str)
{
    uint32 len = static_cast<uint32>(Utils::pr_strlen(str));
    m_Stream->SerializeData((void *)str, len);
}
} // namespace Hazel