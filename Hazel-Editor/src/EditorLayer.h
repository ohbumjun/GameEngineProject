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

		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.f };
		Ref<Texture2D> m_CheckerboardTexture;
		Ref<Texture2D> m_SpriteSheet;

		Ref<SubTexture2D> m_TextureStairs;
		Ref<SubTexture2D> m_TextureTree;
		Ref<SubTexture2D> m_TextureGrass;
		Ref<SubTexture2D> m_TextureBarrel;

		Ref<FrameBuffer> m_FrameBuffer;

		glm::vec2 m_ViewPortSize;

		uint32_t m_MapWidth;
		uint32_t m_MapHeight;
		std::unordered_map<char, Ref<SubTexture2D>> m_TextureMap;
	};
}

