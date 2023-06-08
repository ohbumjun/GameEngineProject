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
	m_CheckerboardTexture = Hazel::Texture2D::Create("assets/textures/sample.png");
}

void SandBox2D::OnDetach()
{
}

void SandBox2D::OnUpdate(Hazel::Timestep ts)
{	
	HZ_PROFILE_FUNCTION();

	// Update
	{
		m_CameraController.OnUpdate(ts);
	}

	// Render
	Hazel::Renderer2D::ResetStats();

	Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
	Hazel::RenderCommand::Clear();

	static float rotation = 0.f;
	rotation += ts * 20.f;

	// Renderer::BeginScene(camera, lights, environment);
	// Scene 을 그리기 위해 필요한 모든 것을 한번에 그려낸다.
	Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

	Hazel::Renderer2D::DrawQuad({ 1.5f, 1.5f, -0.1f }, { 1.5f, 1.5f }, m_CheckerboardTexture, 1.f, { 0.2f, 0.2f, 0.8f, 1.0f });
	Hazel::Renderer2D::DrawRotatedQuad({ 2.5f, 2.5f }, { 1.5f, 1.5f }, rotation, { 0.9f, 0.2f, 0.8f, 1.0f });
	Hazel::Renderer2D::DrawRotatedQuad({ 0.f, 0.f }, { 1.f, 1.f }, 
		 5.f, m_CheckerboardTexture, 2.f, glm::vec4(1.0f, 0.9f, 0.9f, 1.0f));

	Hazel::Renderer2D::EndScene();

	Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

	for (float y = -5.f; y < 5.f; y += 0.5f)
	{
		for (float x = -5.f; x < 5.f; x += 0.5f)
		{
			glm::vec3 color = { (x + 5.f) / 10.f, 0.4f, (y + 5.f) / 10.f };
			Hazel::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, { color, 0.5f });
		}
	}
	Hazel::Renderer2D::EndScene();
}

void SandBox2D::OnEvent(Hazel::Event& event)
{

	m_CameraController.OnEvent(event);
}

void SandBox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");

	auto stats = Hazel::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats  : ");
	ImGui::Text("Draw Calls : %d", stats.DrawCalls);
	ImGui::Text("Quads : %d", stats.QuadCount);
	ImGui::Text("Vertices : %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices : %d", stats.GetTotalIndexCount());

	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	ImGui::End();
}
