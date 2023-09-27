#include "hzpch.h"
#include "SceneSerializer.h"
#include "Entity.h"
#include "Component.h"

#include <yaml-cpp/yaml.h>
#include <fstream>

namespace Hazel
{
	static void SerializeEntityText(YAML::Emitter& out, Entity entity)
	{
		out << YAML::Key << "Entity";
		out << YAML::Value << "12334";

		if (entity.HasComponent<NameComponent>())
		{
			out << YAML::Key << "NameComponent";
			out << YAML::BeginMap;
			auto& tag = entity.GetComponent<NameComponent>().name;
			out << YAML::Key << "Name" << YAML::Value << tag;
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene) :
		m_Scene(scene)
	{
	}

	void SceneSerializer::SerializeText(const std::string& filePath)
	{
		YAML::Emitter out;

		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitied";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		m_Scene->m_Registry.each([&](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get()};

				if (!entity) return;

				SerializeEntityText(out, entity);
			});

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filePath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeBinary(const std::string& filePath)
	{
	}

	bool SceneSerializer::DeserializeText(const std::string& filePath)
	{
	}

	bool SceneSerializer::DeserializeBinary(const std::string& filePath)
	{
	}

}

