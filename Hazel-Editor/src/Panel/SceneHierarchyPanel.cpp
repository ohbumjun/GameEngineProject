#include "hzpch.h"
#include "SceneHierarchyPanel.h"

#include "Hazel/Scene/Entity.h"
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
		ImGui::Begin("SceneHierarchy");

		m_Context->m_Registry.each([&](auto entityID)
			{
				Entity entity{ entityID, m_Context.get() };

				auto& nc = entity.GetComponent<NameComponent>();

				ImGui::Text("%s", nc.name.c_str());
			});


		ImGui::End();
	}
}


