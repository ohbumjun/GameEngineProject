#include "hzpch.h"
#include "SceneSerializer.h"
#include "Entity.h"
#include "Component.h"

#include <fstream>

namespace Hazel
{
	SceneSerializer::SceneSerializer(const Ref<Scene>& scene) :
		m_Scene(scene)
	{
	}

	void SceneSerializer::SerializeText(const std::string& filePath)
	{
		// YAML::Emitter out;
		// 
		// out << YAML::BeginMap;
		// out << YAML::Key << "Scene" << YAML::Value << "Untitied";
		// out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		// 
		// m_Scene->m_Registry.each([&](auto entityID)
		// 	{
		// 		Entity entity = { entityID, m_Scene.get()};
		// 
		// 		if (!entity) return;
		// 
		// 		SerializeEntityText(out, entity);
		// 	});
		// 
		// out << YAML::EndSeq;
		// out << YAML::EndMap;
		// 
		// std::ofstream fout(filePath);
		// fout << out.c_str();
	}

	void SceneSerializer::SerializeBinary(const std::string& filePath)
	{
	}

	bool SceneSerializer::DeserializeText(const std::string& filePath)
	{
		return false;
	}

	bool SceneSerializer::DeserializeBinary(const std::string& filePath)
	{
		return false;
	}

}

