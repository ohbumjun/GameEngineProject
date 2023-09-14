#pragma once

#include "Hazel/Scene/Entity.h"
#include "Hazel/Scene/Scene.h"

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

		Ref<Scene> m_Context;
		Entity m_SelectedEntity;

		friend class Scene;
	};
}



