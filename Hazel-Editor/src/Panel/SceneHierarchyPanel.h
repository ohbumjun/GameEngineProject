#pragma once

#include "Hazel/Scene/Entity.h"
#include "Hazel/Scene/Scene.h"

#include <glm/gtc/type_ptr.hpp>

namespace Hazel
{
	class SceneHierarchyPanel
	{
	public :
		SceneHierarchyPanel() : m_Context(nullptr) {};
		SceneHierarchyPanel(const Ref<Scene>& scene);
		void SetContext(const Ref<Scene>& scene);
		void OnImGuiRender();
	private :
		void drawEntityNode(Entity entity);
		void drawComponents(Entity entity);

		void drawVec3Control(const std::string& lable, glm::vec3& values,
			float resetValues = 0.0f, float columnWidth = 100.f);

		Ref<Scene> m_Context;
		Entity m_SelectedEntity;

		friend class Scene;
	};
}



