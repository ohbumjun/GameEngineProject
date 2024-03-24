#pragma once
#include <Hazel.h>

class EchoTCPClientLayer : public Hazel::Layer
{
public:
    EchoTCPClientLayer() : Layer("IMGUIChat")
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
    static const int BUF_SIZE = 1024;
    // ���� ���̺귯�� �ʱ�ȭ
    WSADATA wsaData;
    SOCKET hClntSock;
    SOCKADDR_IN clntAddr;
};