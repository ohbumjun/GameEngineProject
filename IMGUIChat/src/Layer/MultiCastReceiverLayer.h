#pragma once
#include "ServerInfo.h"
#include <Hazel.h>

class MultiCastReceiverLayer : public Hazel::Layer
{
public:
    MultiCastReceiverLayer() : Layer("MultiCastReceiverLayer")
    {
    }
    ~MultiCastReceiverLayer();
    virtual void OnAttach() override;
    virtual void OnDetach() override;

    void OnUpdate(Hazel::Timestep ts) override;
    void OnEvent(Hazel::Event &event) override;
    virtual void OnImGuiRender() override;
    void ImGuiChatWindow();

private:
    void initializeConnection();
    void receiveResponse();

    static const int BUF_SIZE = 1024;

    // 소켓 라이브러리 초기화
    WSADATA m_WsaData;
    // 멀티캐스트 그룹에 가입하기 위한 구조체
    struct ip_mreq m_JoinAdr; 
    int receiveLen = 0;
    SOCKET m_ReceiverSock; 
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
    char m_RecvBuffer[1024];
};