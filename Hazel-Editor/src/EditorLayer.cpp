#include "EditorLayer.h"
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

namespace Hazel
{

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"),
		m_CameraController(1280.f / 720.f, true)
	{
	}

	void EditorLayer::OnAttach()
	{
		m_CheckerboardTexture = Hazel::Texture2D::Create("assets/textures/sample.png");
		m_SpriteSheet = Hazel::Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");
		// m_TextureStairs				= Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, {7, 6}, {128, 128});
		m_TextureBarrel = Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 8, 2 }, { 128, 128 });
		// m_TextureTree				= Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 2, 1 }, { 128, 128 }, { 1,2 });
		// m_TextureGrass				= Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 1, 11 }, { 128, 128 }, { 1,2 });

		m_TextureMap['D'] = Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 6, 11 }, { 128, 128 });
		m_TextureMap['W'] = Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 11, 11 }, { 128, 128 });

		m_MapWidth = s_mapWidth;
		m_MapHeight = strlen(s_MapTiles) / s_mapWidth;

		Hazel::FrameBufferSpecification fbSpec{};
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_FrameBuffer = Hazel::FrameBuffer::Create(fbSpec);

		m_CameraController.SetZoomLevel(0.25f);
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate(Hazel::Timestep ts)
	{
		HZ_PROFILE_FUNCTION();

		// Update
		{
			m_CameraController.OnUpdate(ts);
		}

		// Reset
		Hazel::Renderer2D::ResetStats();

		{
			m_FrameBuffer->Bind();
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

		m_FrameBuffer->UnBind();
	}

	void EditorLayer::OnEvent(Hazel::Event& event)
	{
		m_CameraController.OnEvent(event);
	}

	void EditorLayer::OnImGuiRender()
	{
		static bool dockingEnabled = true;

		if (dockingEnabled)
		{
			static bool dockspaceOpen = true;
			static bool opt_fullscreen_persistant = true;
			bool opt_fullscreen = opt_fullscreen_persistant;
			static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
			// because it would be confusing to have two docking targets within each others.
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			if (opt_fullscreen)
			{
				ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(viewport->Pos);
				ImGui::SetNextWindowSize(viewport->Size);
				ImGui::SetNextWindowViewport(viewport->ID);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			}

			// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
			if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
				window_flags |= ImGuiWindowFlags_NoBackground;

			// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
			// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
			// all active windows docked into it will lose their parent and become undocked.
			// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
			// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
			ImGui::PopStyleVar();

			if (opt_fullscreen)
				ImGui::PopStyleVar(2);

			// DockSpace
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
					//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

					if (ImGui::MenuItem("Exit")) Hazel::Application::Get().Close();
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			ImGui::Begin("Settings");

			auto stats = Hazel::Renderer2D::GetStats();
			ImGui::Text("Renderer2D Stats:");
			ImGui::Text("Draw Calls: %d", stats.DrawCalls);
			ImGui::Text("Quads: %d", stats.QuadCount);
			ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
			ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
			ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

			{
				// Get rid of padding
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });

				ImGui::Begin("ViewPort");

				ImVec2 viewPortPanelSize = ImGui::GetContentRegionAvail();

				if (m_ViewPortSize != *((glm::vec2*)&viewPortPanelSize))
				{
					// Viewport resize 가 일어난 것이므로, 해당 size 에 맞게 frame buffer 를 다시 생성할 것이다.
					m_FrameBuffer->Resize(viewPortPanelSize.x, viewPortPanelSize.y);
				}
				m_ViewPortSize = { (uint32_t)viewPortPanelSize.x, (uint32_t)viewPortPanelSize.y };

				uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();

				// 기본 imgui : min (0,0) ~ max(1,1)
				// opengl 과 imgui 는 texture uv 의 y 가 반대이다
				// 따라서 이를 적용해주기 위해 min(0,1), ~ max(1,0) 으로 세팅한다.
				ImGui::Image((void*)textureID, ImVec2{ (float)m_ViewPortSize.x, (float)m_ViewPortSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

				ImGui::End();
				ImGui::PopStyleVar();
			}
			ImGui::End();

			ImGui::End();
		}
		// docking 설정이 안되어 있을 때
		else
		{
			ImGui::Begin("Settings");

			auto stats = Hazel::Renderer2D::GetStats();
			ImGui::Text("Renderer2D Stats:");
			ImGui::Text("Draw Calls: %d", stats.DrawCalls);
			ImGui::Text("Quads: %d", stats.QuadCount);
			ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
			ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

			ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

			uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
			// uint32_t textureID = m_CheckerboardTexture->GetRendererID();

			// 기본 imgui : min (0,0) ~ max(1,1)
			// opengl 과 imgui 는 texture uv 의 y 가 반대이다
			// 따라서 이를 적용해주기 위해 min(0,1), ~ max(1,0) 으로 세팅한다.
			// ImGui::Image((void*)textureID, ImVec2{ (float)m_ViewPortSize .x, (float)m_ViewPortSize .y}, ImVec2{0, 1}, ImVec2{1, 0});
			ImGui::Image((void*)textureID, ImVec2{ (float)1280, (float)720}, ImVec2{0, 1}, ImVec2{1, 0});
			ImGui::End();
		}
	}

}
