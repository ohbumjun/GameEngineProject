#pragma once
#include <Hazel.h>

class ChatServerLayer : public Hazel::Layer
{
public:
    ChatServerLayer() : Layer("ChatServerLayer")
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
    SOCKET hServSock;   // 서버 소켓
    SOCKET hClntSock;   // 수락된 클라이언트 소켓
    SOCKADDR_IN servAddr, clntAddr;
};