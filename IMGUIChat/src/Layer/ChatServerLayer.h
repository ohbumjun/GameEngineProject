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

    // ���� ���̺귯�� �ʱ�ȭ
    WSADATA wsaData;
    SOCKET hServSock;   // ���� ����
    SOCKET hClntSock;   // ������ Ŭ���̾�Ʈ ����
    SOCKADDR_IN servAddr, clntAddr;
};