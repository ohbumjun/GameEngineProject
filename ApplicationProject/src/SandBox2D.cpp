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

	// Init Particle
	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 1.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };
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
	{
		Hazel::Renderer2D::ResetStats();

		Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
		Hazel::RenderCommand::Clear();

		static float rotation = 0.f;
		rotation += ts * 20.f;

		// Renderer::BeginScene(camera, lights, environment);
		// Scene 을 그리기 위해 필요한 모든 것을 한번에 그려낸다.
		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

		Hazel::Renderer2D::DrawQuad({ 1.5f, 1.5f, -0.1f }, { 1.5f, 1.5f }, m_CheckerboardTexture, 1.f, { 0.2f, 0.2f, 0.8f, 1.0f });
		Hazel::Renderer2D::DrawRotatedQuad({ 2.5f, 2.5f }, { 1.5f, 1.5f }, glm::radians(rotation), { 0.9f, 0.2f, 0.8f, 1.0f });
		Hazel::Renderer2D::DrawRotatedQuad({ 0.f, 0.f }, { 1.f, 1.f }, glm::radians(5.f), m_CheckerboardTexture, 2.f, glm::vec4(1.0f, 0.9f, 0.9f, 1.0f));

		// Hazel::Renderer2D::EndScene();

		// Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

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

	{
		if (Hazel::Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_LEFT))
		{
			auto [x, y] = Hazel::Input::GetMousePosition();
			auto width = Hazel::Application::Get().GetWindow().GetWidth();
			auto height = Hazel::Application::Get().GetWindow().GetHeight();

			auto bounds = m_CameraController.GetBounds();
			auto pos		= m_CameraController.GetCamera().GetPosition();
			x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
			y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
			m_Particle.Position = { x + pos.x, y + pos.y };
			
			for (int i = 0; i < 5; i++)
			{
				m_ParticleSystem.Emit(m_Particle);
			}
		}

		m_ParticleSystem.OnUpdate(ts);
		m_ParticleSystem.OnRender(m_CameraController.GetCamera());
	}
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
