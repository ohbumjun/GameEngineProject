#include "hzpch.h"
#include "SceneHierarchyPanel.h"

#include "Hazel/Scene/Component/Component.h"
#include "Hazel/Scene/Component/SpriteRenderComponent.h"
#include "Hazel/Scene/Component/NameComponent.h"
#include "Hazel/Scene/Component/TransformComponent.h"
#include "Hazel/Scene/Component/CameraComponent.h"
#include "Hazel/Scene/Component/NativeScriptComponent.h"

#include <imgui/imgui.h>

namespace Hazel
{

	static void DrawVec3Control(const std::string& lable, glm::vec3& values, 
		float resetValues = 0.0f, float columnWidth = 100.f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[1];


		ImGui::PushID(lable.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(lable.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 1.f, 0 });

		// ImGui::PushItemWidth(3, ImGui::CalcItemWidth());
		// float lineHeight = ImGui::GetTextLineHeightWithSpacing();
		float lineHeight = ImGui::GetFontSize() + 2.f * ImGui::GetStyle().FramePadding.y;
		ImVec2 buttonSize = { lineHeight + 2.f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.3f, 0.15f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.5f, 0.15f, 1.0f));

		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize)) {
			values.x = resetValues;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		// ImGui::PushItemWidth(lineHeight * 4.0f); // Adjust the width as needed
		ImGui::PushItemWidth(lineHeight * 2.0f); // Adjust the width as needed
		ImGui::DragFloat("##X", &values.x, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.8f, 0.15f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.8f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.7f, 0.5f, 1.0f));

		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize)) {
			values.y = resetValues;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::PushItemWidth(lineHeight * 2.0f); // Adjust the width as needed
		ImGui::DragFloat("##Y", &values.y, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();


		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.8f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.1f, 0.8f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.1f, 0.8f, 1.0f));

		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize)) {
			values.z = resetValues;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::PushItemWidth(lineHeight * 2.0f); // Adjust the width as needed
		ImGui::DragFloat("##Z", &values.z, 0.1f);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}
	template<typename T, typename UFunction>
	static void DrawComponent(const std::string& name, Entity entity, UFunction uFunc)
	{
		// const ImGuiTreeNodeFlags treeNodeFlag = ImGuiTreeNodeFlags_DefaultOpen |
		const ImGuiTreeNodeFlags treeNodeFlag =
			ImGuiTreeNodeFlags_AllowItemOverlap |
			ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_FramePadding;

		float lineHeight = ImGui::GetFontSize() + 2.f * ImGui::GetStyle().FramePadding.y;
		ImVec2 buttonSize = { lineHeight, lineHeight };

		if (entity.HasComponent<T>())
		{
			// transform 조정
			auto& component = entity.GetComponent<T>();

			ImVec2 contentRegion = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 2.f, 2.f });

			ImGui::Separator();

			// 해당 영역을 선택해서 열어야만 조정 가능하다
			bool opened = ImGui::TreeNodeEx((void*)typeid(T).hash_code(),
				treeNodeFlag, name.c_str());

			ImGui::SameLine(contentRegion.x - lineHeight * 0.5f);

			bool removeComponent = false;

			if (ImGui::Button("+", buttonSize))
				ImGui::OpenPopup("ComponentSettings");

			ImGui::PopStyleVar();

			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove Component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (opened)
			{
				uFunc(component);

				ImGui::TreePop();
			}

			if (removeComponent)
			{
				entity.RemoveComponent<T>();
			}
		}
	}

	SceneHierarchyPanel::SceneHierarchyPanel(const  std::weak_ptr<Scene>& scene) :
		m_Context(scene)
	{
		// SetContext(scene);
	}

	void SceneHierarchyPanel::SetContext(const  std::weak_ptr<Scene>& scene)
	{
		m_Context = scene;

		m_SelectedEntity = {};
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		Ref<Scene> sceneContext = m_Context.lock();

		if (!sceneContext)
		{
			assert(false);
		}

		{
			// Scene Hierarchy
			ImGui::Begin("SceneHierarchy");

			// Scene Name
			{
				// 여기서 name 을 수정할 수 있게 한다.
				const std::string& name = sceneContext->GetName();

				char buffer[256];
				memset(buffer, 0, sizeof(buffer));
				strcpy_s(buffer, sizeof(buffer), name.c_str());

				ImGui::Text("SceneName");

				ImGui::SameLine();

				if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
				{
					sceneContext->SetName(std::string(buffer));
				}
			}
		

			sceneContext->m_Registry.each([&](auto entityID)
				{
					Entity entity{ entityID, m_Context.lock().get()};
					drawEntityNode(entity);
				});

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_SelectedEntity = {};

			// Right Click on Black Space
			if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
			{
				if (ImGui::MenuItem("Create Empty Entity"))
					sceneContext->CreateEntity();
		
				ImGui::EndPopup();
			}

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
		Ref<Scene> sceneContext = m_Context.lock();

		if (!sceneContext)
		{
			assert(false);
		}

		std::string& name = const_cast<std::string&>(
			entity.GetComponent<NameComponent>().GetName());

		// ImGuiTreeNodeFlags_OpenOnArrow : arrow 를 클릭해야만 해당 내용 모두 펼쳐서 볼 수 있다.
		//	ImGuiTreeNodeFlags_Selected			: selected 된 채로 보이게 된다.
		ImGuiTreeNodeFlags flags = 
			(m_SelectedEntity == entity ?  ImGuiTreeNodeFlags_Selected : 0) |
			ImGuiTreeNodeFlags_OpenOnArrow;

		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		
		bool isOpened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, name.c_str());
		bool entityDeleted = false;

		if (ImGui::IsItemClicked())
		{
			m_SelectedEntity = entity;
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}
		
		if (isOpened)
		{
			{
				// Tree 안에 Tree Node 가 보일 수 있게 한다.
				ImGuiTreeNodeFlags flags = (m_SelectedEntity == entity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
				bool isOpened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, name.c_str());
				flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
				if (isOpened)
				{

					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			sceneContext->DestroyEntity(entity);

			// m_SelectedEntity ? == 현재 클릭된 Entity 의 Component 목록을 보고 있었다는 의미
			// m_SelectedEntity 정보를 비워준다.
			if (m_SelectedEntity == entity)
				m_SelectedEntity = {};
		}
	}

	void SceneHierarchyPanel::drawComponents(Entity entity)
	{
		if (entity.HasComponent<NameComponent>())
		{
			// 여기서 name 을 수정할 수 있게 한다.
			std::string& name = const_cast<std::string&>(
				entity.GetComponent<NameComponent>().GetName());

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), name.c_str());

			if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
			{
				name = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		{
			if (ImGui::Button("Add Component"))
				ImGui::OpenPopup("AddComponent");

			if (ImGui::BeginPopup("AddComponent"))
			{
				if (ImGui::MenuItem("Camera"))
				{
					m_SelectedEntity.AddComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("SpriteRenderer"))
				{
					m_SelectedEntity.AddComponent<SpriteRenderComponent>();
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}

		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component) {
			DrawVec3Control("Translation", component.GetTranslationRef());
			glm::vec3 degree = glm::degrees(component.GetRotationRef());
			DrawVec3Control("Rotation", degree);
			// component.Rotation = glm::radians(degree);
			component.SetRotation(glm::radians(degree));
			DrawVec3Control("Scale", component.GetScaleRef());
		});

		DrawComponent<CameraComponent>("Sprite", entity, [](auto& component) {
			SceneCamera& camera = const_cast<SceneCamera&>(
				component.GetCamera());

			ImGui::Checkbox("Primary", &component.GetPrimaryRef());

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
				float orthoSize = camera.GetOrthoGraphicSize();
				float orthoNear = camera.GetOrthographicNear();
				float orthoFar = camera.GetOrthographicFar();

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

			ImGui::Checkbox("Fixed Aspect Ratio", &component.GetFixedAspectRatioRef());
		});

		DrawComponent<SpriteRenderComponent>("Sprite", entity, [](auto& component) {
			ImGui::ColorEdit4("Color", glm::value_ptr(component.GetColorRef()));
		});

	}

}


