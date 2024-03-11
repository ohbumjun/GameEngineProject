#pragma once
#include <Hazel.h>



class IMGUIChatLayer : public Hazel::Layer
{
public:
    IMGUIChatLayer() : Layer("IMGUIChat")
    {
    }

    void OnUpdate(Hazel::Timestep ts) override
    {
    }

    void OnEvent(Hazel::Event &event) override
    {
    }

    virtual void OnImGuiRender() override
    {
        ImGuiChatWindow();
    }
    
    void ImGuiChatWindow();

    void ImGuiConnectWindow();

private:
};