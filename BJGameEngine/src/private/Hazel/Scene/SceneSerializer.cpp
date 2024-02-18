#include "hzpch.h"
#include "Hazel/Scene/SceneSerializer.h"
#include "Hazel/Scene/Entity.h"
#include "Hazel/Core/Serialization/JsonSerializer.h"
#include "Hazel/FileSystem/FileMemory.h"

namespace Hazel
{
	SceneSerializer::SceneSerializer(const Ref<Scene>& scene) :
		m_Scene(scene)
	{
	}

	void SceneSerializer::SerializeText(const std::string& filePath)
	{
		JsonSerializer writer;
		m_Scene->Serialize(&writer);
		const std::string& result = writer.GetFinalResult();

		// 해당 경로의 파일은 항상 생성하는 방향으로 진행할 것이다.
		FileMemory fileMemory(filePath.c_str(), FileOpenMode::CREATE);

		fileMemory.SerializeData(result.c_str(), result.length());
		fileMemory.FlushToFile();
		fileMemory.End();
	}

	void SceneSerializer::SerializeBinary(const std::string& filePath)
	{
	}

	void SceneSerializer::DeserializeText(const std::string& filePath)
	{
		// 해당 경로의 파일은 항상 생성하는 방향으로 진행할 것이다.
		FileMemory fileMemory(filePath.c_str(), FileOpenMode::OPEN, FileAccessMode::READ_ONLY);

		char* buffer = new char[fileMemory.GetDataLength() + 1];
		fileMemory.DeserializeData(buffer, fileMemory.GetDataLength());

		// 마지막 NULL 넣어주기
		buffer[fileMemory.GetDataLength()] = NULL;
		
		JsonSerializer reader(buffer);
		m_Scene->Deserialize(&reader);

		delete [] buffer;
	}

	void SceneSerializer::DeserializeBinary(const std::string& filePath)
	{
	}
};

