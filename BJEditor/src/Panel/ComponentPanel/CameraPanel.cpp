#include "hzpch.h"
#include "CameraPanel.h"
#include <imgui.h>
#include "Hazel/Scene/Component/CameraComponent.h"
#include "Renderer/Camera/EditorCamera.h"

void HazelEditor::CameraPanel::DrawCameraComponent(Hazel::Component& inputComponent)
{
	/*
	HZ_CORE_ASSERT(entity.HasComponent<Hazel::CameraComponent>(), "Camera Comp Does Not Exist");
	Hazel::CameraComponent& component = entity.GetComponent<Hazel::CameraComponent>();
	*/
		
	HZ_CORE_ASSERT(inputComponent.GetType() == Reflection::GetTypeID<Hazel::CameraComponent>(),
		"component should be camera");

	Hazel::CameraComponent& component = static_cast<Hazel::CameraComponent&>(inputComponent);
	Hazel::SceneCamera& camera = const_cast<Hazel::SceneCamera&>(component.GetCamera());

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
				camera.SetProjectionType((Hazel::SceneCamera::ProjectionType)i);
			}

			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	};

	if (camera.GetProjectionType() == Hazel::SceneCamera::ProjectionType::Projective)
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
	else if (camera.GetProjectionType() == Hazel::SceneCamera::ProjectionType::Orthographic)
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

}

void HazelEditor::CameraPanel::DrawEditorCamera(Hazel::EditorCamera* editorCamera)
{
	const ImGuiTreeNodeFlags treeNodeFlag =
		ImGuiTreeNodeFlags_AllowItemOverlap |
		ImGuiTreeNodeFlags_Framed |
		ImGuiTreeNodeFlags_FramePadding;

	float lineHeight = ImGui::GetFontSize() + 2.f * ImGui::GetStyle().FramePadding.y;
	ImVec2 buttonSize = { lineHeight, lineHeight };

	float dist = editorCamera->GetDistance();
	float perspectiveFov = editorCamera->GetPerspectiveFov();
	float perspectiveNear = editorCamera->GetPerspectiveNear();
	float perspectiveFar = editorCamera->GetPerspectiveFar();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 2.f, 2.f });

	ImGui::Separator();

	if (ImGui::DragFloat("Dist", &dist))
	{
		editorCamera->SetDistance(dist);
	}

	if (ImGui::DragFloat("Fov", &perspectiveFov))
	{
		editorCamera->SetPerspectiveFov(perspectiveFov);
	}

	if (ImGui::DragFloat("Near", &perspectiveNear))
	{
		editorCamera->SetPerspectiveNearClip(perspectiveNear);
	}

	if (ImGui::DragFloat("Far", &perspectiveFar))
	{
		editorCamera->SetPerspectiveFarClip(perspectiveFar);
	}

	ImGui::PopStyleVar();
}
