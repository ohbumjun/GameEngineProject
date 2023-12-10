#pragma once

#include <Hazel.h>
#include "ParticleSystem.h"
#include "Panel/SceneHierarchyPanel.h"

namespace HazelEditor
{
	class EditorLayer : public Hazel::Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		void OnUpdate(Hazel::Timestep ts) override;
		void OnEvent(Hazel::Event& event) override;
		virtual void OnImGuiRender() override;

	private:
		bool OnKeyPressed(Hazel::KeyPressedEvent& e);

		void NewScene();
		void OpenScene();
		void SaveSceneAs();

		void ResetEditorLayer(std::weak_ptr<Hazel::Scene> scene);

		void prepareDockSpace();
		void drawMenuBar();
		void drawPanels();
		void drawSetting();
		void drawViewPort();

		// Temp
		Hazel::OrthographicCameraController m_CameraController;

		Hazel::Ref<Hazel::Texture2D> m_CheckerboardTexture;
		Hazel::Ref<Hazel::Texture2D> m_SpriteSheet;

		Hazel::Ref<Hazel::SubTexture2D> m_TextureStairs;
		Hazel::Ref<Hazel::SubTexture2D> m_TextureTree;
		Hazel::Ref<Hazel::SubTexture2D> m_TextureGrass;
		Hazel::Ref<Hazel::SubTexture2D> m_TextureBarrel;

		Hazel::Ref<Hazel::Scene> m_ActiveScene;

		// Hazel::Entity m_SquareEntity;
		// Hazel::Entity m_CameraEntity;
		// Hazel::Entity m_SecondCameraEntity;

		bool m_PrimaryCamera = false;

		Hazel::Ref<Hazel::FrameBuffer> m_FrameBuffer;

		bool m_ViewportFocused  = false;
		bool m_ViewportHovered  = false;
		bool m_VieportInteracted = false;

		glm::vec2 m_ViewportSize = { 0,0 };

		uint32_t m_MapWidth;
		uint32_t m_MapHeight;
		std::unordered_map<char, Hazel::Ref<Hazel::SubTexture2D>> m_TextureMap;

		ParticleProps m_Particle;
		ParticleSystem m_ParticleSystem;

		int m_GizmoType = -1;

		// Panels
		Hazel::Ref<Hazel::SceneHierarchyPanel> m_SceneHierachyPanel;
	};
}

