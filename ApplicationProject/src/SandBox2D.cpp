#include "SandBox2D.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>

SandBox2D::SandBox2D()
	: Layer("SandBox2D"),
		m_CameraController(1200.f / 720.f, true)
{
}

void SandBox2D::OnAttach()
{
	
}

void SandBox2D::OnDetach()
{
}

void SandBox2D::OnUpdate(Hazel::Timestep ts)
{	
	// Update
	m_CameraController.OnUpdate(ts);

	// Render
	Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
	Hazel::RenderCommand::Clear();

	// Renderer::BeginScene(camera, lights, environment);
	// Scene 을 그리기 위해 필요한 모든 것을 한번에 그려낸다.
	Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

	// TODO : Shader Set Mat4, Set Float4 (Add Functions For these)
	Hazel::Renderer2D::DrawQuad({ 0.f, 0.f }, { 1.f, 1.f }, { 0.8f, 0.2f, 0.3f, 1.0f });
	Hazel::Renderer2D::DrawQuad({ 5.f, 5.f }, { 1.5f, 1.5f }, { 0.2f, 0.2f, 0.3f, 1.0f });

	Hazel::Renderer2D::EndScene();
}

void SandBox2D::OnEvent(Hazel::Event& event)
{

	m_CameraController.OnEvent(event);
}

void SandBox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");

	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	ImGui::End();
}
