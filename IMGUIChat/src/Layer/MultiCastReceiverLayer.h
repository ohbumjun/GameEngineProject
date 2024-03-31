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

    // ���� ���̺귯�� �ʱ�ȭ
    WSADATA m_WsaData;
    // ��Ƽĳ��Ʈ �׷쿡 �����ϱ� ���� ����ü
    struct ip_mreq m_JoinAdr; 
    int receiveLen = 0;
    SOCKET m_ReceiverSock; 
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
    char m_RecvBuffer[1024];
};