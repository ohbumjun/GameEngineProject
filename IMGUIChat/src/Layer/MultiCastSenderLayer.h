#pragma once
#include "ServerInfo.h"
#include <Hazel.h>
/*
* (참고)
* 멀티캐스트란, UDP 기반 그룹 중심 전송 방식으로, 멀티캐스트 
* 그룹에 속한 호스트들에게 데이터를 전송하는 방식이다.
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

    //  child process 들을 등록시킬 job object
    HANDLE hJob;

    // 소켓 라이브러리 초기화
    WSADATA wsaData;
    SOCKET hSenderSock;
    SOCKADDR_IN senderAddr;

    bool connected = false;
    char recvBuffer[1024];
    int recvBufferSize = 0;

    std::string m_InputText;
    int m_ClientCount = 0;
};