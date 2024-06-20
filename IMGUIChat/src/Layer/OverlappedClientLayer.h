#pragma once
#include "Hazel/Core/ID/UUID.h"
#include "Hazel/Core/Thread/Thread.h"
#include "Hazel/Core/Thread/ThreadUtil.h"
#include "ServerInfo.h"
#include <Hazel.h>

class OverlappedClientLayer : public Hazel::Layer
{
public:
    OverlappedClientLayer() : Layer("OverlappedClientLayer")
    {
    }
    ~OverlappedClientLayer();
    virtual void OnAttach() override;
    virtual void OnDetach() override;

    void OnUpdate(Hazel::Timestep ts) override;
    void OnEvent(Hazel::Event &event) override;
    virtual void OnImGuiRender() override;
    void ImGuiChatWindow();

private:
    void initialize();
    void receiveMessage();

    static const int BUF_SIZE = 1024;

    WSADATA wsaData;
    SOCKET hSocket;
    int receiveLen = 0;
    SOCKADDR_IN sendAddr;
    WSAEVENT evObj;
    WSAOVERLAPPED overlapped;
    std::string inputText;
    std::vector<std::string> m_ReceivedMessage;
    Hazel::Thread m_ReceiveThread;
    Hazel::CRIC_SECT *m_CricSect;
};