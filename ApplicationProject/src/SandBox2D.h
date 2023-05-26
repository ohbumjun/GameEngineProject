#pragma once

#include <Hazel.h>

class SandBox2D : public Hazel::Layer
{
public:
	SandBox2D();
	virtual ~SandBox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	void OnUpdate(Hazel::Timestep ts) override;
	void OnEvent(Hazel::Event& event) override;
	virtual void OnImGuiRender() override;

private:
	// Temp
	Hazel::Ref<Hazel::VertexArray> m_SquareArray;
	Hazel::OrthographicCameraController m_CameraController;
	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.f };
	Hazel::Ref<Hazel::Shader> m_FlatShader;
	Hazel::Ref<Hazel::Texture2D> m_CheckerboardTexture;

};

