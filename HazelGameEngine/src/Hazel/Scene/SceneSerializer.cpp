#include "hzpch.h"
#include "SceneSerializer.h"
#include "Entity.h"
#include "Hazel/Core/Serialization/JsonSerializer.h"
#include <fstream>

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

