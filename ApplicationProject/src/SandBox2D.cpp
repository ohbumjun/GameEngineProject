#include "SandBox2D.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include <chrono>

template<typename Fn>
class Timer
{
public :
	Timer(const char* name, Fn&& func) :
		m_Name(name), m_Stopped(false), m_Func(std::move(func))
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}
	~Timer()
	{
		if (!m_Stopped)
			Stop();
	}
	void Stop()
	{
		auto endTimePoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		long long end  = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();
	
		m_Stopped = true;

		float duration = (end - start) * 0.001f;

		m_Func({ m_Name, duration });
	}
private :
	const char* m_Name;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
	bool m_Stopped;
	Fn   m_Func;
};

// timer       : Timer 객체
// __LINE__ : line number
#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](ProfileResult profileResult) {m_ProfileResult.push_back(profileResult);})

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
	// Update
	{
		PROFILE_SCOPE("CameraController::Update");
		m_CameraController.OnUpdate(ts);
	}

	// Render
	Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
	Hazel::RenderCommand::Clear();

	// Renderer::BeginScene(camera, lights, environment);
	// Scene 을 그리기 위해 필요한 모든 것을 한번에 그려낸다.
	Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

	// TODO : Shader Set Mat4, Set Float4 (Add Functions For these)
	Hazel::Renderer2D::DrawQuad({ 0.f, 0.f }, { 1.f, 1.f }, { 0.8f, 0.2f, 0.3f, 1.0f });
	Hazel::Renderer2D::DrawQuad({ 1.f, 1.f }, { 1.5f, 1.5f }, { 0.2f, 0.2f, 0.8f, 1.0f });
	Hazel::Renderer2D::DrawQuad({ -1.f, -1.f }, { 10.f, 10.f }, m_CheckerboardTexture);

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

	{
		// Print out Profileresult
		for (auto& result : m_ProfileResult)
		{
			char label[60];
			strcpy(label, result.name);
			strcat(label, "  %.2f ms");
			ImGui::Text(label, result.time);
		}

		m_ProfileResult.clear();
	}


	ImGui::End();
}
