#pragma once

#include "Scene.h"

namespace Hazel
{
	class SceneSerializer
	{
	public :
		SceneSerializer(const Ref<Scene>& scene);

		void SerializeText(const std::string& filePath);
		// runtime
		void SerializeBinary(const std::string& filePath);

		void DeserializeText(const std::string& filePath);
		void DeserializeBinary(const std::string& filePath);

	private:
		Ref<Scene> m_Scene;
	};
}


