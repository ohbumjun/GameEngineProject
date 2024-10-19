#pragma once

#include <Hazel.h>

namespace HazelEditor
{
class ProjectSelectLayer : public Hazel::Layer
{
public:
    ProjectSelectLayer();
    virtual ~ProjectSelectLayer() = default;

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

    void prepareDockSpace();
    void drawPanels();

    // Hazel::Ref<ProjectSelectPanel> m_ProjectSelectPanel;
};
} // namespace HazelEditor
