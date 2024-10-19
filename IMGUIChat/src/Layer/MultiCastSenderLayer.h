#pragma once
#include "ServerInfo.h"
#include <Hazel.h>
/*
* (����)
* ��Ƽĳ��Ʈ��, UDP ��� �׷� �߽� ���� �������, ��Ƽĳ��Ʈ 
* �׷쿡 ���� ȣ��Ʈ�鿡�� �����͸� �����ϴ� ����̴�.
*/
class MultiCastSenderLayer : public Hazel::Layer
{
public:
    MultiCastSenderLayer() : Layer("MultiCastSenderLayer")
    {
    }
    ~MultiCastSenderLayer();
    virtual void OnAttach() override;
    virtual void OnDetach() override;

    void OnUpdate(Hazel::Timestep ts) override;
    void OnEvent(Hazel::Event &event) override;
    virtual void OnImGuiRender() override;
    void ImGuiChatWindow();
    void ImGuiCreateReceiverWindow();

private:
    static const int BUF_SIZE = 1024;
    static const int TTL = 64;

    void createClient();
    void initialize();
    void finalize();

    //  child process ���� ��Ͻ�ų job object
    HANDLE hJob;

    // ���� ���̺귯�� �ʱ�ȭ
    WSADATA wsaData;
    SOCKET hSenderSock;
    SOCKADDR_IN senderAddr;

    bool connected = false;
    char recvBuffer[1024];
    int recvBufferSize = 0;

    std::string m_InputText;
    int m_ClientCount = 0;
};