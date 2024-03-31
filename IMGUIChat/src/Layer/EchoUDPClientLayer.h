#pragma once
#include "ServerInfo.h"
#include <Hazel.h>
class EchoUDPClientLayer : public Hazel::Layer
{
public:
    EchoUDPClientLayer() : Layer("EchoUDPClientLayer")
    {
    }
    ~EchoUDPClientLayer();
    virtual void OnAttach() override;
    virtual void OnDetach() override;

    void OnUpdate(Hazel::Timestep ts) override;
    void OnEvent(Hazel::Event &event) override;
    virtual void OnImGuiRender() override;
    void ImGuiChatWindow();

private:
    static const int BUF_SIZE = 1024;

    void initialize();

    // ���� ���̺귯�� �ʱ�ȭ
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
    std::string m_InputText;
};