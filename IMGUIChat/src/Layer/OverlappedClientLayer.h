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
    void receiveMessage(Hazel::UUID threadId);
    void receiveConnection();

    static const int BUF_SIZE = 1024;

    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN sendAddr;
    WSABUF dataBuf;
    WSAEVENT evObj;
    WSAOVERLAPPED overlapped;
    std::string inputText;
};