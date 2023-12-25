#pragma once

#include <string>
#include "Hazel/Scene/Entity.h"
#include <glm/glm.hpp>
#include <imgui/imgui.h>

namespace HazelEditor {

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
	static void DrawComponent(const std::string& name, Hazel::Entity entity, UFunction uFunc)
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

}