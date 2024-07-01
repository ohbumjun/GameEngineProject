#include "Hazel/Scene/SceneSerializer.h"
#include "Hazel/Core/Serialization/JsonSerializer.h"
#include "Hazel/FileSystem/FileStream.h"
#include "Hazel/Scene/Entity.h"
#include "hzpch.h"

namespace Hazel
{
SceneSerializer::SceneSerializer(const Ref<Scene> &scene) : m_Scene(scene)
{
}

void SceneSerializer::SerializeText(const std::string &filePath)
{
    JsonSerializer writer;
    m_Scene->Serialize(&writer);
    const std::string &result = writer.GetFinalResult();

    // 해당 경로의 파일은 항상 생성하는 방향으로 진행할 것이다.
    FileStream FileStream(filePath.c_str(), FileOpenMode::CREATE);

    FileStream.SerializeData(result.c_str(), result.length());
    FileStream.FlushToFile();
    FileStream.End();
}

void SceneSerializer::SerializeBinary(const std::string &filePath)
{
}

void SceneSerializer::DeserializeText(const std::string &filePath)
{
    // 해당 경로의 파일은 항상 생성하는 방향으로 진행할 것이다.
    FileStream FileStream(filePath.c_str(),
                          FileOpenMode::OPEN,
                          FileAccessMode::READ_ONLY);

    char *buffer = new char[FileStream.GetDataLength() + 1];
    FileStream.DeserializeData(buffer, FileStream.GetDataLength());

    // 마지막 NULL 넣어주기
    buffer[FileStream.GetDataLength()] = NULL;

    JsonSerializer reader(buffer);
    m_Scene->Deserialize(&reader);

    delete[] buffer;
}

void SceneSerializer::DeserializeBinary(const std::string &filePath)
{
}
}; // namespace Hazel
