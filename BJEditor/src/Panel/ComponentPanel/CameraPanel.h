#pragma once

#include "Hazel/Scene/Component/Component.h"
#include "Hazel/Scene/SceneCamera.h"

namespace Hazel {
	class EditorCamera;
}

namespace HazelEditor
{
	class CameraPanel
	{
	public :
		/*
		���� ���� 
		- HazelEditor::DrawComponent<CameraComponent>("Camera", entity, &HazelEditor::CameraPanel::DrawCameraComponent);
		  �� ���� DrawComponent �Լ��� ���ڿ����� ����Ѵ�.
		*/
		static void DrawCameraComponent(Hazel::Component& inputComponent);
		static void DrawEditorCamera(Hazel::EditorCamera* editorCamera);
	};
}

