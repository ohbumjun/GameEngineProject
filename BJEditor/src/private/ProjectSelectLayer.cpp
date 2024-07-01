#include "ProjectSelectLayer.h"

// 3rd
#include "ImGuizmo.h"
#include "imgui.h"
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

// Editor
#include "File/PathManager.h"
#include "Panel/ComponentPanel/CameraPanel.h"
#include "Panel/Utils/PanelUtils.h"

// Engine
#include "Hazel/FileSystem/DirectorySystem.h"
#include "Hazel/Math/Math.h"
#include "Hazel/Scene/SceneSerializer.h"
#include "Hazel/Utils/PlatformUtils.h"

#include "Hazel/Scene/Component/CameraComponent.h"
#include "Hazel/Scene/Component/Collider/BoxCollider2DComponent.h"
#include "Hazel/Scene/Component/Collider/CircleCollider2DComponent.h"
#include "Hazel/Scene/Component/Identifier/NameComponent.h"
#include "Hazel/Scene/Component/TransformComponent.h"
#include "Platform/OpenGL/OpenGLShader.h"


namespace HazelEditor
{
extern const std::filesystem::path g_AssetPath;

ProjectSelectLayer::ProjectSelectLayer() : Layer("ProjectSelectLayer")
{
}

void ProjectSelectLayer::OnAttach()
{
    static std::string resourceRootPath = RESOURCE_ROOT;
    //	m_CheckerboardTexture = Hazel::TextureManager::CreateTexture2D("assets/textures/sample.png");
    // m_SpriteSheet = Hazel::TextureManager::CreateTexture2D("assets/game/textures/RPGpack_sheet_2X.png");

    // m_ProjectSelectPanel = Hazel::CreateRef<HazelEditor::ProjectSelectPanel>();
}
void ProjectSelectLayer::OnDetach()
{
}
void ProjectSelectLayer::OnUpdate(Hazel::Timestep ts)
{
    HZ_PROFILE_FUNCTION();

    // Reset
    Hazel::Renderer2D::ResetStats();
    Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.f});
    Hazel::RenderCommand::Clear();
}
void ProjectSelectLayer::OnEvent(Hazel::Event &event)
{
    // m_CameraController.OnEvent(event);
    //
    // if (m_SceneState == SceneState::Edit)
    // {
    //     m_EditorCamera.OnEvent(event);
    // }
    //
    // Hazel::EventDispatcher dispatcher(event);
    // dispatcher.Dispatch<Hazel::KeyPressedEvent>(
    //     HZ_BIND_EVENT_FN(ProjectSelectLayer::onKeyPressed));
    // dispatcher.Dispatch<Hazel::MouseButtonPressedEvent>(
    //     HZ_BIND_EVENT_FN(ProjectSelectLayer::onMouseButtonPressed));
}
void ProjectSelectLayer::OnImGuiRender()
{
    HZ_PROFILE_FUNCTION();

    prepareDockSpace();
    drawPanels();
}
void ProjectSelectLayer::prepareDockSpace()
{
    // DockSpace
    // Note: Switch this to true to enable dockspace
    static bool dockspaceOpen = true;
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
                        ImGuiWindowFlags_NoNavFocus;
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
    ImGuiIO &io = ImGui::GetIO();
    ImGuiStyle &style = ImGui::GetStyle();
    float minWindowWidth = style.WindowMinSize.x;
    style.WindowMinSize.x = 370.f;

    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    style.WindowMinSize.x = minWindowWidth;
}
void ProjectSelectLayer::drawPanels()
{
    // Panels
    // m_ProjectSelectPanel->OnImGuiRender();
}
} // namespace HazelEditor