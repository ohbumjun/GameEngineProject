#pragma once
#include <Hazel.h>
#include "ServerInfo.h"

class EchoTCPServerLayer : public Hazel::Layer
{
public:
    EchoTCPServerLayer() : Layer("EchoTCPServerLayer")
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
    SOCKET hServSock;       // ���� ����
    SOCKET hClntSock[5];    // ������ Ŭ���̾�Ʈ ����
    int         hClntStrLen[5]; // ������ Ŭ���̾�Ʈ ����
    /*
    IPv4 �� �ּ������� ��� ���� ���ǵ� ����ü

    struct sockaddr_in
    {
        sa_family_t sin_family;  // �ּ� ü��
        uint16_t sin_port;          // 16��Ʈ TCP/UDP PORT ��ȣ
        struct in_addr sin_addr; // 32 ��Ʈ IP �ּ� : ��� ��Ʈ��ũ ����Ʈ ������ �����ؾ���
        char sin_zero[8]
    }

    struct in_addr
    {
	    in_addr_t       s_addr;     // 32��Ʈ IPv4 ���ͳ� �ּ�
    }
    */
    SOCKADDR_IN servAddr, clntAddr;

    
bool connected = false;
    char recvBuffer[1024];
    int recvBufferSize = 0;

    // ImGui-related variables
    // ImGuiTextBuffer chatHistory;
    bool showConnectWindow = true;
    char username[32] = "";
    char messageBuffer[256] = "";
};