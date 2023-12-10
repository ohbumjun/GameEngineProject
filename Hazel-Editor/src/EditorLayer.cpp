#include "EditorLayer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"
#include "ImGuizmo.h"
#include "File/PathManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Hazel/FileSystem/DirectorySystem.h"
#include "Hazel/Scene/SceneSerializer.h"
#include "Hazel/Math/Math.h"
#include "Hazel/Scene/Component/CameraComponent.h"
#include "Hazel/Scene/Component/TransformComponent.h"
#include "Hazel/Utils/PlatformUtils.h"

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

// 임시 코드
static std::string secondCameraName = "Second Camera Entity";

class CameraTestController : public Hazel::ScriptableEntity
{
public:
	void OnCreate()
	{
		bool h = true;

	}
	void OnUpdate(Hazel::Timestep ts)
	{
		bool h = true;

	}
	void OnDestroy()
	{
		bool h = true;
	}
};

namespace HazelEditor
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
		// m_TextureTree				= Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 2, 1 }, { 128, 128 }, { 1,2 });
		// m_TextureGrass				= Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 1, 11 }, { 128, 128 }, { 1,2 });
		m_TextureBarrel = Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 8, 2 }, { 128, 128 });

		m_TextureMap['D'] = Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 6, 11 }, { 128, 128 });
		m_TextureMap['W'] = Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 11, 11 }, { 128, 128 });

		m_MapWidth = s_mapWidth;
		m_MapHeight = (uint32_t)strlen(s_MapTiles) / s_mapWidth;

		Hazel::FrameBufferSpecification fbSpec{};
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_FrameBuffer = Hazel::FrameBuffer::Create(fbSpec);

		m_ActiveScene = Hazel::CreateRef<Hazel::Scene>("ActiveScene");
		auto squareEntity = m_ActiveScene->CreateEntity("Square Entity");
		squareEntity.AddComponent<Hazel::SpriteRenderComponent>(glm::vec4{ 0.f, 1.f, 0.f, 1.f });
	
		auto secondSquareEntity = m_ActiveScene->CreateEntity("Second Square Entity");
		secondSquareEntity.AddComponent<Hazel::SpriteRenderComponent>(glm::vec4{ 1.f, 0.f, 0.f, 1.f });

		auto cameraEntity = m_ActiveScene->CreateEntity("Main Camera Entity");
		cameraEntity.AddComponent<Hazel::CameraComponent>(glm::ortho(-16.f, 16.f, -9.f, 9.f, -1.f, 1.f));

		auto secondCameraEntity = m_ActiveScene->CreateEntity(secondCameraName);
		auto& secCc = secondCameraEntity.AddComponent<Hazel::CameraComponent>(glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f));
		secCc.SetPrimary(false);

		m_CameraController.SetZoomLevel(0.25f);
		secondCameraEntity.AddComponent<Hazel::NativeScriptComponent>().Bind<CameraTestController>();
	
		// Panels
		m_SceneHierachyPanel = Hazel::CreateRef<Hazel::SceneHierarchyPanel>();
		m_SceneHierachyPanel->SetContext(m_ActiveScene);
}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate(Hazel::Timestep ts)
	{
		HZ_PROFILE_FUNCTION();

		// Resize
		{
			Hazel::FrameBufferSpecification spec = m_FrameBuffer->GetSpecification();

			if (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y)
			{
				/*
				FrameBuffer 에 그려진 내용은 CameraController 의 Camera 로 부터
				보여지는 내용이다.
				전체 Window 를 Resize 하던, 해당 ViewPort 만 Resize 하던
				어떤 이유로 인해 ViewPort  크기가 변하면
				그에 맞게 Viewport 크기도 변화해야 할 것이다.
				*/
				m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
				
				// CameraController 의 Camera Projection Matrix 정보 다시 세팅
				m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
				
				// Scene 에 있는 모든 CameraComponent 의 Camera 정보 다시 세팅
				m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			}
		}

		// Update
		// m_ViewportFocused ? void ImGuiLayer::OnEvent 함수 참고
		if (m_VieportInteracted)
		{
			m_CameraController.OnUpdate(ts);
		}

		// Reset
		Hazel::Renderer2D::ResetStats();
		m_FrameBuffer->Bind();
		Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
		Hazel::RenderCommand::Clear();

		// Render
		m_ActiveScene->OnUpdate(ts);

		m_FrameBuffer->UnBind();
	}

	void EditorLayer::OnEvent(Hazel::Event& event)
	{
		m_CameraController.OnEvent(event);

		Hazel::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<Hazel::KeyPressedEvent>(HZ_BIND_EVENT_FN(EditorLayer::OnKeyPressed));

	}

	void EditorLayer::OnImGuiRender()
	{
		HZ_PROFILE_FUNCTION();

		prepareDockSpace();
		drawMenuBar();
		drawPanels();
		drawSetting();
		drawViewPort();
	}

	bool EditorLayer::OnKeyPressed(Hazel::KeyPressedEvent& e)
	{
		// Shortcuts
		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Hazel::Input::IsKeyPressed(Hazel::Key::LeftControl) || 
			Hazel::Input::IsKeyPressed(Hazel::Key::RightControl);
		bool shift = Hazel::Input::IsKeyPressed(Hazel::Key::LeftShift) 
			|| Hazel::Input::IsKeyPressed(Hazel::Key::RightShift);
		switch (e.GetKeyCode())
		{
		case Hazel::Key::N:
		{
			if (control)
			{
				NewScene();
			}

			break;
		}
		case Hazel::Key::O:
		{
			if (control)
			{
				OpenScene();
			}

			break;
		}
		case Hazel::Key::S:
		{
			if (control && shift)
			{
				SaveSceneAs();
			}

			break;
		}
		// Gizmos
		case Hazel::Key::Q:
			m_GizmoType = -1;
			break;
		case Hazel::Key::W:
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case Hazel::Key::E:
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		case Hazel::Key::R:
			m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}

		return true;
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = Hazel::CreateRef<Hazel::Scene>("Scene");
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierachyPanel->SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		// std::string filepath = Hazel::FileChooser::OpenFile("Hazel Scene (*.hazel)\0*.hazel\0");
		std::string filepath = Hazel::FileChooser::OpenFile("Hazel Scene (*.scene)\0*.scene\0");
		
		if (!filepath.empty())
		{
			// 기존  Scene 을 지운다.
			m_ActiveScene = Hazel::CreateRef<Hazel::Scene>();
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierachyPanel->SetContext(m_ActiveScene);
			Hazel::SceneSerializer serializer(m_ActiveScene);
			serializer.DeserializeText(filepath.c_str());

			ResetEditorLayer(m_ActiveScene);
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		// std::string filepath = Hazel::FileChooser::SaveFile("Hazel Scene (*.hazel)\0*.hazel\0");
		std::string filepath = Hazel::FileChooser::SaveFile("Hazel Scene (*.scene)\0*.scene\0");
		if (!filepath.empty())
		{
			Hazel::SceneSerializer serializer(m_ActiveScene);
			serializer.SerializeText(filepath);
		}
	}
	void EditorLayer::ResetEditorLayer(std::weak_ptr<Hazel::Scene> scene)
	{
		Hazel::Entity secondCameraEntity = scene.lock().get()->GetEntityByName(secondCameraName);

		secondCameraEntity.GetComponent<Hazel::NativeScriptComponent>().Bind<CameraTestController>();
	}
	void EditorLayer::prepareDockSpace()
	{
		// DockSpace
			// Note: Switch this to true to enable dockspace
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
		ImGuiStyle& style = ImGui::GetStyle();
		float minWindowWidth = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWindowWidth;

	}
	void EditorLayer::drawMenuBar()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("Exit"))
				{
					Hazel::Application::Get().Close();
				}
				if (ImGui::MenuItem("New", "Ctrl+N"))
					NewScene();

				if (ImGui::MenuItem("Serialize"))
				{
					Hazel::SceneSerializer serializer(m_ActiveScene);
					serializer.SerializeText("assets/scenes/Example.hazel");
				}
				if (ImGui::MenuItem("Open Scene...", "Ctrl+O"))
				{
					OpenScene();
				}
				if (ImGui::MenuItem("Deserialize"))
				{
					Hazel::SceneSerializer serializer(m_ActiveScene);
					// serializer.DeserializeText("assets/scenes/Example.hazel");
				}
				if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
				{
					SaveSceneAs();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}
	void EditorLayer::drawPanels()
	{	
		// Panels
		m_SceneHierachyPanel->OnImGuiRender();
	}
	void EditorLayer::drawSetting()
	{
		// Settings
		ImGui::Begin("Settings");

		auto stats = Hazel::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		{
			// auto& cameraComp = m_SecondCameraEntity.GetComponent<Hazel::CameraComponent>();
			// auto& camera = cameraComp.GetCamera();
			// float orthoSize = camera.GetOrthoGraphicSize();
			// 
			// if (ImGui::DragFloat("Second Camera Ortho Size", &orthoSize))
			// {
			// 	const_cast<Hazel::SceneCamera&>(camera).SetOrthoGraphicSize(orthoSize);
			// }
		}

		ImGui::End();
	}
	void EditorLayer::drawViewPort()
	{
		// ViewPort
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");

		/*
		* 해당 viewport 를 한번 클릭하면 true. 다른 곳을 클릭하면 그때 false 가 되지만
		* 다른 곳을 클릭하지 않는 이상, 한번 클릭하면 계속 true 가 된다는 것이다.
		*/
		bool prevFocused = m_ViewportFocused;
		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();

		/*
		즉, 만약 viewport 가 focus 혹은 hover 되었다면, ImguiLayer 단에서 event 를 처리하고 싶다.
		즉, 다른 layer 에서는 event 를 처리하지 않게 하고 싶은 것이다.
		ex) 다른 imgui 창에 마우스를 대고 있으면, scroll 을 해도, Viewport 상에서 camera 의
			 zoom in, zoom out 이 일어나지 않게 하고 싶은 것이다.
			 반대로, Viewport 에 마우스를 대고 있으면 scroll 했을 때, 다른 imgui 창에
			 어떤 영향도 안 미치고 싶다.

		BlockEvent(true) 라는 것은, ImGuiLayer 가 event 를 인식하게 한다는 것이고
		다른 Layer 들은 event 를 인식하지 않게 한다는 것이다.
		ImGuiLayer 가 event 를 인식한다는 것은, ImGui 창에서 scoll 을 하면
		ImGui 창이 scroll 된다는 것이고
		다른 Layer 들은 scroll 관련 영향을 안받는다는 것이다.

		EditorLayer 의 CameraController 가 event 를 인식하지 않는 것.
		Camera 의 zoom in ,zoom out 이 일어나지 않는다는 것.

		반대로 BlockEvent(false) 라는 것은, ImGuiLayer 가 event 를 인식하지 않는 것
		다른 Layer 들은 event 를 인식하는 것
		Editor Layer 에서 CameraController 가 event 를 인식하고
		Camera 의 zoom in ,out 이 일어나게 하는 것

		따라서 Focus 되거나 Hover 가 되면 , ImGuiLayer 는 Event 를 인식하지 않게 하고
		EditorLayer 가 event 를 인식할 수 있게 해야 한다.
		ImGuiLayer 의 m_BlockEvents 를 false 로 만들어야 한다.
		Focus 혹은 Hover 둘 중 하나라도 되면 false 이고
		Focuse 혹은 Hover 둘다 안되어야만 true 이다.
		*/

		bool viewPortFocusedNow = false;

		if (m_ViewportFocused && prevFocused == false)
		{
			viewPortFocusedNow = true;
		}

		m_VieportInteracted = m_ViewportHovered || viewPortFocusedNow;

		// Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);
		// Hazel::Application::Get().GetImGuiLayer()->BlockEvents(!m_VieportInteracted);
		Hazel::Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();

		// uint32_t textureID = m_CheckerboardTexture->GetRendererID();
		ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		// Gizmos
		Hazel::Entity selectedEntity = m_SceneHierachyPanel.get()->GetSelectedEntity();

		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			// Set ViewPort
			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			// Camera
			auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
			const auto& camera = cameraEntity.GetComponent<Hazel::CameraComponent>().GetCamera();
			const glm::mat4& cameraProjection = camera.GetProjection();

			// cameraView : inverse transform of camera world transform
			glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<Hazel::TransformComponent>().GetTransform());

			// Entity transform
			auto& tc = selectedEntity.GetComponent<Hazel::TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			// Snapping
			bool snap = Hazel::Input::IsKeyPressed(Hazel::Key::LeftControl);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale

			// Snap to 45 degrees for rotation
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			// Guizmo 를 Render 하는 함수
			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				// 실제 ImGuizom 화살표 등이 나와서 우리가 사용할 때만
				// transformation  정보들을 update 해주고자 한다.

				glm::vec3 translation, rotation, scale;
				Hazel::Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.GetRotation();
				tc.SetTranslation(translation);
				tc.SetRotation(tc.GetRotation() + deltaRotation);
				tc.SetScale(scale);
				// tc.Translation = translation;
				// tc.Rotation += deltaRotation;
				// tc.Scale = scale;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}
}
