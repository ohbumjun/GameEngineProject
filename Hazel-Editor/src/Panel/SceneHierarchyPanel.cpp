#include "hzpch.h"
#include "SceneHierarchyPanel.h"

#include "Hazel/Scene/Component.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Hazel
{
	static void DrawVec3Control(const std::string& lable, glm::vec3& values, 
		float resetValues = 0.0f, float columnWidth = 100.f)
	{
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(lable.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 1.f, 0 });

		// ImGui::PushItemWidth(3, ImGui::CalcItemWidth());
		// float lineHeight = ImGui::GetTextLineHeightWithSpacing();
		float lineHeight = ImGui::GetFontSize() + 2.f * ImGui::GetStyle().FramePadding.y;
		ImVec2 buttonSize = { lineHeight + 4.f, lineHeight};

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.3f, 0.15f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.5f, 0.15f, 1.0f));

		if (ImGui::Button("X", buttonSize)) {
			values.x = resetValues;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		// ImGui::PushItemWidth(lineHeight * 4.0f); // Adjust the width as needed
		ImGui::PushItemWidth(lineHeight * 4.0f); // Adjust the width as needed
		ImGui::DragFloat("##X", &values.x, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.8f, 0.15f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.8f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.7f, 0.5f, 1.0f));

		if (ImGui::Button("Y", buttonSize)) {
			values.y = resetValues;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::PushItemWidth(lineHeight * 4.0f); // Adjust the width as needed
		ImGui::DragFloat("##Y", &values.y, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();


		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.8f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.1f, 0.8f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.1f, 0.8f, 1.0f));
		if (ImGui::Button("Z", buttonSize)) {
			values.z = resetValues;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::PushItemWidth(lineHeight * 4.0f); // Adjust the width as needed
		ImGui::DragFloat("##Z", &values.z, 0.1f);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);
	}

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

			if (ImGui::InputText("Name", buffer, sizeof(buffer)))
			{
				name = std::string(buffer);
			}
		}

		if (entity.HasComponent<TransformComponent>())
		{
			// 해당 영역을 선택해서 열어야만 조정 가능하다
			if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(),
				ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{
				// transform 조정
				auto& transformComp = entity.GetComponent<TransformComponent>();

				// ImGui::DragFloat3("Position", glm::value_ptr(transformComp.Translation), 0.1f);
				DrawVec3Control("Translation", transformComp.Translation);

				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<CameraComponent>())
		{
			// 해당 영역을 선택해서 열어야만 조정 가능하다
			if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(),
				ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			{
				// transform 조정
				auto& cameraComp = entity.GetComponent<CameraComponent>();
				auto& camera = cameraComp.camera;

				ImGui::Checkbox("Primary", &cameraComp.isPrimary);

				const char* projectionTypeStrings[] = { "Projection", "Orthographic" };
				const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];

				if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
				{
					for (int i = 0; i < 2; ++i)
					{
						bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];

						if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
						{
							currentProjectionTypeString = projectionTypeStrings[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGui::EndCombo();
				};

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Projective)
				{
					float perspectiveFov = glm::degrees(camera.GetPerspectiveFov());
					float perspectiveNear = camera.GetPerspectiveNear();
					float perspectiveFar = camera.GetPerspectiveFar();

					if (ImGui::DragFloat("Fov", &perspectiveFov))
					{
						camera.SetPerspectiveFov(glm::radians(perspectiveFov));
					}

					if (ImGui::DragFloat("Near", &perspectiveNear))
					{
						camera.SetPerspectiveNearClip(perspectiveNear);
					}

					if (ImGui::DragFloat("Far", &perspectiveFar))
					{
						camera.SetPerspectiveFarClip(perspectiveFar);
					}
				}
				else if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize		= camera.GetOrthoGraphicSize();
					float orthoNear		= camera.GetOrthographicNear();
					float orthoFar			= camera.GetOrthographicFar();

					if (ImGui::DragFloat("Size", &orthoSize))
					{
						camera.SetOrthoGraphicSize(orthoSize);
					}

					if (ImGui::DragFloat("Near", &orthoNear))
					{
						camera.SetOrthographicNearClip(orthoNear);
					}

					if (ImGui::DragFloat("Far", &orthoFar))
					{
						camera.SetOrthographicFarClip(orthoFar);
					}
				}

				ImGui::Checkbox("Fixed Aspect Ratio", &cameraComp.isFixedAspectRatio);

				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<SpriteRenderComponent>())
		{
			// 해당 영역을 선택해서 열어야만 조정 가능하다
			if (ImGui::TreeNodeEx((void*)typeid(SpriteRenderComponent).hash_code(),
				ImGuiTreeNodeFlags_DefaultOpen, "Sprite Color"))
			{
				// transform 조정
				auto& sprite = entity.GetComponent<SpriteRenderComponent>();

				ImGui::ColorEdit4("Color", glm::value_ptr(sprite.color));

				ImGui::TreePop();
			}
		}
	}
}


