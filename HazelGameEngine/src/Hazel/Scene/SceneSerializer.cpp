#include "hzpch.h"
#include "SceneSerializer.h"
#include "Entity.h"
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

		// 해당 경로의 파일은 항상 생성하는 방향으로 진행할 것이다.
		FileMemory fileMemory(filePath.c_str(), FileOpenMode::CREATE);
		fileMemory.FlushToFile();
	}

	void SceneSerializer::SerializeBinary(const std::string& filePath)
	{
	}

	bool SceneSerializer::DeserializeText(const std::string& filePath)
	{
		JsonSerializer reader;

		return false;
	}

	bool SceneSerializer::DeserializeBinary(const std::string& filePath)
	{
		return false;
	}

}

