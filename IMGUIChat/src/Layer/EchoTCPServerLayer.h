#pragma once
#include <Hazel.h>
#include "ServerInfo.h"

class EchoTCPServerLayer : public Hazel::Layer
{
public:
    EchoTCPServerLayer() : Layer("EchoTCPServerLayer")
    {
    }
    ~EchoTCPServerLayer();
    virtual void OnAttach() override;
    virtual void OnDetach() override;

    void OnUpdate(Hazel::Timestep ts) override;
    void OnEvent(Hazel::Event &event) override;
    virtual void OnImGuiRender() override;
    void ImGuiChatWindow();
    void ImGuiConnectWindow();
    void ImGuiCreateClientWindow();

private:
    void createClient();
    void initializeConnection();
    void acceptConnection();

    static const int BUF_SIZE = 1024;

    // ���� ���̺귯�� �ʱ�ȭ
    WSADATA m_WsaData;
    SOCKET m_InitServSock;       // ���� ����
    SOCKET m_ClntSocks[5];    // ������ Ŭ���̾�Ʈ ����
    int         m_ClntStrLen[5]; 
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
    SOCKADDR_IN m_ServAddr, m_ClntAddr;

    bool m_Connected = false;
    char m_RecvBuffer[1024];
    int m_RecvBufferSize = 0;
    int m_ClntAddrSize = 0;

    // ImGui-related variables
    // ImGuiTextBuffer chatHistory;
    bool m_ShowConnectWindow = true;
    char m_Username[32] = "";
    char m_MessageBuffer[256] = "";
    
private:
    std::unordered_map<DWORD /*Process ID*/, PROCESS_INFORMATION> m_Pids;
};