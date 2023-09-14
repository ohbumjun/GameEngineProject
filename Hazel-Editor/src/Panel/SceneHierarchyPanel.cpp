#include "hzpch.h"
#include "SceneHierarchyPanel.h"

#include "Hazel/Scene/Component.h"

#include <imgui/imgui.h>

namespace Hazel
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene) :
		m_Context(scene)
	{
		// SetContext(scene);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene)
	{
		m_Context = scene;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		{
			// Scene Hierarchy
			ImGui::Begin("SceneHierarchy");

			m_Context->m_Registry.each([&](auto entityID)
				{
					Entity entity{ entityID, m_Context.get() };
					drawEntityNode(entity);
				});

			ImGui::End();
		}

		{
			// Property
			ImGui::Begin("Properties");

			if (m_SelectedEntity)
			{
				drawComponents(m_SelectedEntity);
			}

			ImGui::End();
		}

	}

	// draw selectable list of entities
	void SceneHierarchyPanel::drawEntityNode(Entity entity)
	{
		auto& name = entity.GetComponent<NameComponent>().name;

		// ImGuiTreeNodeFlags_OpenOnArrow : arrow 를 클릭해야만 해당 내용 모두 펼쳐서 볼 수 있다.
		//	ImGuiTreeNodeFlags_Selected			: selected 된 채로 보이게 된다.
		ImGuiTreeNodeFlags flags = 
			(m_SelectedEntity == entity ?  ImGuiTreeNodeFlags_Selected : 0) |
			ImGuiTreeNodeFlags_OpenOnArrow;
		
		bool isOpened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, name.c_str());
		
		if (ImGui::IsItemClicked())
		{
			m_SelectedEntity = entity;
		}
		
		if (isOpened)
		{
			{
				// Tree 안에 Tree Node 가 보일 수 있게 한다.
				ImGuiTreeNodeFlags flags = (m_SelectedEntity == entity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
				bool isOpened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, name.c_str());

				if (isOpened)
				{

					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}
	}

	void SceneHierarchyPanel::drawComponents(Entity entity)
	{
		if (entity.HasComponent<NameComponent>())
		{
			// 여기서 name 을 수정할 수 있게 한다.
			auto& name = entity.GetComponent<NameComponent>().name;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), name.c_str());

			if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
			{
				name = std::string(buffer);
			}
		}

		if (entity.HasComponent<TransformComponent>())
		{

		}

	}
}


