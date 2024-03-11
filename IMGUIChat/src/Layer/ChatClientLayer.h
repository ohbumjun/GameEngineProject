#pragma once
#include <Hazel.h>

class ChatClientLayer : public Hazel::Layer
{
public:
    ChatClientLayer() : Layer("IMGUIChat")
    {
    }
    virtual void OnAttach() override;
    virtual void OnDetach() override;

    void OnUpdate(Hazel::Timestep ts) override;
    void OnEvent(Hazel::Event &event) override;
    virtual void OnImGuiRender() override
    {
        ImGuiChatWindow();
    }
    
    void ImGuiChatWindow();

    void ImGuiConnectWindow();

private:
    // 소켓 라이브러리 초기화
    WSADATA wsaData;
    SOCKET hClntSock;
    SOCKADDR_IN clntAddr;
};