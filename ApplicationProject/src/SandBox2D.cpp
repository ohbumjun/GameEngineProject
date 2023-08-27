#include "SandBox2D.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>

// 24 wide map
static const uint32_t s_mapWidth = 24;
static const char* s_MapTiles = 
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWDWWWWWWWWDWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWDWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWDWWWWW"
"WWWWWDWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWTWWWWWW"
"WWWWWWWWWWWWWWWWDWWWWWWW"
"WWWWWWWWWDWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWDWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWDWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWDWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW";

SandBox2D::SandBox2D()
	: Layer("SandBox2D"),
		m_CameraController(1200.f / 720.f, true)
{
}

void SandBox2D::OnAttach()
{
	m_CheckerboardTexture = Hazel::Texture2D::Create("assets/textures/sample.png");
	m_SpriteSheet				= Hazel::Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");
	// m_TextureStairs				= Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, {7, 6}, {128, 128});
	m_TextureBarrel				= Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, {8, 2}, {128, 128});
	// m_TextureTree				= Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 2, 1 }, { 128, 128 }, { 1,2 });
	// m_TextureGrass				= Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 1, 11 }, { 128, 128 }, { 1,2 });

	m_TextureMap['D'] = Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 6, 11 }, { 128, 128 });
	m_TextureMap['W'] = Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 11, 11 }, { 128, 128 });

	m_MapWidth = s_mapWidth;
	m_MapHeight = strlen(s_MapTiles) / s_mapWidth;

	// Init Particle
	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 5.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };

	m_CameraController.SetZoomLevel(0.25f);
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

	// Reset
	{
		Hazel::Renderer2D::ResetStats();

		Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
		Hazel::RenderCommand::Clear();
	}

	// Render
#if 0
	{


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
#endif

	{
		// Particle 들을 별도의 Batch 처리로 진행한다.
		// 즉, 위쪽의 일반 Render Batch 처리 한번.
		// 이후 Particle Render Batch 처리 한번. 이런 식으로 진행하는 것이다.
		if (Hazel::Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_LEFT))
		{
			auto [x, y] = Hazel::Input::GetMousePosition();
			auto width = Hazel::Application::Get().GetWindow().GetWidth();
			auto height = Hazel::Application::Get().GetWindow().GetHeight();

			auto bounds = m_CameraController.GetBounds();
			auto pos		= m_CameraController.GetCamera().GetPosition();
			x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
			y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();

			// x, y : Camera  Bound  내에서의 위치
			// pos : 전체  World 상에서 Camera 의 위치 
			m_Particle.Position = { x + pos.x, y + pos.y };
			
			// 1 Frame 마다, 한번의 클릭 , 20개의 Particle 새로 생성
			for (int i = 0; i < 20; i++)
			{
				m_ParticleSystem.Emit(m_Particle);
			}
		}

		m_ParticleSystem.OnUpdate(ts);
		m_ParticleSystem.OnRender(m_CameraController.GetCamera());
	}

	{
		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

		for (uint32_t y = 0; y < m_MapHeight; ++y)
		{
			for (uint32_t x = 0; x < m_MapWidth; ++x)
			{
				char tileType = s_MapTiles[x + y * m_MapWidth];

				Hazel::Ref<Hazel::SubTexture2D> texture;

				if (m_TextureMap.find(tileType) == m_TextureMap.end())
				{
					texture = m_TextureBarrel;
				}
				else
				{
					texture = m_TextureMap[tileType];
				}

				Hazel::Renderer2D::DrawQuad({ x - m_MapWidth / 2.f, y - m_MapHeight / 2.f, -0.1f }, { 1.f, 1.f }, texture);
			}
		}

		// Hazel::Renderer2D::DrawQuad({ 4.5f, 1.5f, -0.1f }, { 1.5f, 1.5f }, m_TextureStairs, 1.f, { 0.2f, 0.2f, 0.8f, 1.0f });
		// Hazel::Renderer2D::DrawQuad({ 2.5f, 1.5f, -0.1f }, { 1.5f, 1.5f }, m_TextureBarrel, 1.f, { 0.2f, 0.2f, 0.8f, 1.0f });
		// Hazel::Renderer2D::DrawQuad({ 1.5f, 1.5f, -0.1f }, { 1.5f, 1.5f }, m_TextureTree, 1.f, { 0.2f, 0.2f, 0.8f, 1.0f });
		// Hazel::Renderer2D::DrawQuad({ 0.5f, 1.5f, -0.1f }, { 1.5f, 1.5f }, m_TextureGrass, 1.f, { 0.2f, 0.2f, 0.8f, 1.0f });
		Hazel::Renderer2D::EndScene();
	}
}

void SandBox2D::OnEvent(Hazel::Event& event)
{
	m_CameraController.OnEvent(event);
}

void SandBox2D::OnImGuiRender()
{
	

	{
		static bool dockSpaceOpen = true;
		bool* p_open = &dockSpaceOpen;

		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", p_open, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
				ImGui::MenuItem("Padding", NULL, &opt_padding);
				ImGui::Separator();

				if (ImGui::MenuItem("Exit")) Hazel::Application::Get().Close();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// Window 를 DockSpace 시키기 위해서는
		// 해당 Window 가 DockSpace {} 안에서 그려지도록 해야한다.
		{
			ImGui::Begin("Settings");

			auto stats = Hazel::Renderer2D::GetStats();
			ImGui::Text("Renderer2D Stats  : ");
			ImGui::Text("Draw Calls : %d", stats.DrawCalls);
			ImGui::Text("Quads : %d", stats.QuadCount);
			ImGui::Text("Vertices : %d", stats.GetTotalVertexCount());
			ImGui::Text("Indices : %d", stats.GetTotalIndexCount());

			ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

			uint32_t textureID = m_SpriteSheet->GetRendererID();
			ImGui::Image((void*)textureID, ImVec2{64.f, 64.f});

			ImGui::End();
		}

		ImGui::End();
	}
}
