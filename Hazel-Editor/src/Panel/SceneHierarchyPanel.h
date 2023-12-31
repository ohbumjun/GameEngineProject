#pragma once

#include "Hazel/Scene/Entity.h"
#include "Hazel/Scene/Scene.h"

#include <glm/gtc/type_ptr.hpp>

// Editor 에 있는 코드이지만, Scene 에 접근할 수 있게 하기 위해
// 예외적으로 하나만 HazelEditor 가 아니라
// Hazel Editor Namespace 로 설정한다.
namespace Hazel
{
	class SceneHierarchyPanel
	{
	public :
		SceneHierarchyPanel() {};
		// SceneHierarchyPanel(const Ref<Scene>& scene);
		SceneHierarchyPanel(const std::weak_ptr<Scene>& scene);
		// void SetContext(const Ref<Scene>& scene);
		void SetContext(const  std::weak_ptr<Scene>& scene);
		void SetSelectedEntity(Entity entity);
		void OnImGuiRender();
		Entity GetSelectedEntity() const
		{
			return m_SelectedEntity;
		}
	private :
		void drawEntityNode(Entity entity);
		void drawComponents(Entity entity);
		void drawNameComponent(Entity entity);
		void drawTransformComponent(Entity entity);
		void drawCameraComponent(Entity entity);
		void drawRendererComponent(Entity entity);
		void drawColliderComponent(Entity entity);
		void drawRigidBodyComponent(Entity entity);

		template<typename T>
		void displayAddComponentEntry(const std::string& entryName);


		void drawAddComponentPanel();
		std::weak_ptr<Scene> m_Context;
		Entity m_SelectedEntity;

		friend class Scene;
	};
}



