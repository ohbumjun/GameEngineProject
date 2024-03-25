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
    // 소켓 라이브러리 초기화
    WSADATA wsaData;
    SOCKET hServSock;       // 서버 소켓
    SOCKET hClntSock[5];    // 수락된 클라이언트 소켓
    int         hClntStrLen[5]; // 수락된 클라이언트 소켓
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