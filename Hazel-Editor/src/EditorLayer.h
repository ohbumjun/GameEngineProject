#pragma once

#include <Hazel.h>

namespace Hazel
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		void OnUpdate(Timestep ts) override;
		void OnEvent(Event& event) override;
		virtual void OnImGuiRender() override;

	private:
		// Temp
		OrthographicCameraController m_CameraController;

		Ref<Texture2D> m_CheckerboardTexture;
		Ref<Texture2D> m_SpriteSheet;

		Ref<SubTexture2D> m_TextureStairs;
		Ref<SubTexture2D> m_TextureTree;
		Ref<SubTexture2D> m_TextureGrass;
		Ref<SubTexture2D> m_TextureBarrel;

		Ref<Scene> m_ActiveScene;

		Entity m_SquareEntity;

		Ref<FrameBuffer> m_FrameBuffer;

		bool m_ViewportFocused  = false;
		bool m_ViewportHovered  = false;
		bool m_VieportInteracted = false;

		glm::vec2 m_ViewportSize = { 0,0 };

		uint32_t m_MapWidth;
		uint32_t m_MapHeight;
		std::unordered_map<char, Ref<SubTexture2D>> m_TextureMap;
	};
}

