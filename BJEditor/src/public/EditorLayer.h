#pragma once

#include "Hazel/Asset/Image/SubTexture2D.h"
#include "Panel/ContentBrowserPanel.h"
#include "Panel/SceneHierarchyPanel.h"
#include "ParticleSystem.h"
#include "Renderer/Buffer/FrameBuffer.h"
#include "Renderer/Camera/EditorCamera.h"
#include "Renderer/Camera/OrthographicCameraController.h"
#include <Hazel.h>

namespace HazelEditor
{
class EditorLayer : public Hazel::Layer
{
public:
    EditorLayer();
    virtual ~EditorLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    void OnUpdate(Hazel::Timestep ts) override;
    void OnEvent(Hazel::Event &event) override;
    virtual void OnImGuiRender() override;

private:
    enum class SceneState
    {
        Edit = 0,
        Play = 1,
        Simulate = 2
    };
    bool onKeyPressed(Hazel::KeyPressedEvent &e);
    bool onMouseButtonPressed(Hazel::MouseButtonPressedEvent &e);
    void newScene();
    void openScene();
    void openScene(const std::filesystem::path &path);
    void saveSceneAs();
    void saveScene();

    void serializeScene(Hazel::Ref<Hazel::Scene> scene,
                        const std::filesystem::path &path);
    void onSceneSimulate();

    void prepareDockSpace();
    void drawMenuBar();
    void drawPanels();
    void drawSetting();
    void drawViewPort();
    void pickMouse2D();

    void onOverlayRender();
    void onScenePlay();
    void onSceneStop();
    void onDuplicateEntity();


    // UI Panels
    void uI_Toolbar();

    // Temp
    Hazel::OrthographicCameraController m_CameraController;

    /*
		Hazel::Ref<Hazel::Texture2D> m_CheckerboardTexture;
		Hazel::Ref<Hazel::Texture2D> m_SpriteSheet;

		Hazel::Ref<Hazel::SubTexture2D> m_TextureStairs;
		Hazel::Ref<Hazel::SubTexture2D> m_TextureTree;
		Hazel::Ref<Hazel::SubTexture2D> m_TextureGrass;
		Hazel::Ref<Hazel::SubTexture2D> m_TextureBarrel;
		*/

    // runtime scene
    Hazel::Ref<Hazel::Scene> m_ActiveScene;

    // 평소 editor scene
    Hazel::Ref<Hazel::Scene> m_EditorScene;

    std::filesystem::path m_EditorScenePath;

    Hazel::Entity m_HoveredEntity;

    bool m_PrimaryCamera = false;

    Hazel::Ref<Hazel::FrameBuffer> m_FrameBuffer;

    bool m_ViewportFocused = false;
    bool m_ViewportHovered = false;
    bool m_VieportInteracted = false;

    bool m_ShowPhysicsColliders = false;
    bool m_HighlightSelectEntity = false;

    glm::vec2 m_ViewportSize = {0, 0};
    glm::vec2 m_ViewportBounds[2];

    uint32_t m_MapWidth;
    uint32_t m_MapHeight;
    std::unordered_map<char, Hazel::Ref<Hazel::SubTexture2D>> m_TextureMap;

    ParticleProps m_Particle;
    ParticleSystem m_ParticleSystem;
    Hazel::EditorCamera m_EditorCamera;


    // Editor resources
    Hazel::Ref<Hazel::Texture2D> m_IconPlay, m_IconSimulate, m_IconStop;

    SceneState m_SceneState = SceneState::Edit;

    int m_GizmoType = -1;

    // Panels
    Hazel::Ref<Hazel::SceneHierarchyPanel> m_SceneHierachyPanel;
    Hazel::Ref<ContentBrowserPanel> m_ContentBrowserPanel;
};
} // namespace HazelEditor
