#pragma once
#include "ServerInfo.h"
#include <Hazel.h>
class EchoTCPClientLayer : public Hazel::Layer
{
public:
    EchoTCPClientLayer() : Layer("EchoTCPClientLayer")
    {
    }
    virtual void OnAttach() override;
    virtual void OnDetach() override;

    void OnUpdate(Hazel::Timestep ts) override;
    void OnEvent(Hazel::Event &event) override;
    virtual void OnImGuiRender() override;
    void ImGuiChatWindow();
    void ImGuiConnectWindow();

private:
    static const int BUF_SIZE = 1024;

    void connectServer();

    // 소켓 라이브러리 초기화
    WSADATA wsaData;
    SOCKET hClntSock;
    SOCKADDR_IN clntAddr;
    
    bool connected = false;
    char recvBuffer[1024];
    int recvBufferSize = 0;

    // ImGui-related variables
    // ImGuiTextBuffer chatHistory;
    bool showConnectWindow = true;
    char username[32] = "";
    char messageBuffer[256] = "";
};