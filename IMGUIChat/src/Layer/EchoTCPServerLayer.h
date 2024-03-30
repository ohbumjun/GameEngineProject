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

    // 소켓 라이브러리 초기화
    WSADATA m_WsaData;
    SOCKET m_InitServSock;       // 서버 소켓
    SOCKET m_ClntSocks[5];    // 수락된 클라이언트 소켓
    int         m_ClntStrLen[5]; 
    /*
    IPv4 의 주소정보를 담기 위해 정의된 구조체

    struct sockaddr_in
    {
        sa_family_t sin_family;  // 주소 체계
        uint16_t sin_port;          // 16비트 TCP/UDP PORT 번호
        struct in_addr sin_addr; // 32 비트 IP 주소 : 모두 네트워크 바이트 순서로 저장해야함
        char sin_zero[8]
    }

    struct in_addr
    {
	    in_addr_t       s_addr;     // 32비트 IPv4 인터넷 주소
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