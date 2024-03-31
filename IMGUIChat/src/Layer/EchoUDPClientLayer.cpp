#include "EchoUDPClientLayer.h"
#include "Util/Util.h"

EchoUDPClientLayer::~EchoUDPClientLayer()
{
    // 생성된 소켓 라이브러리 해제
    closesocket(hClntSock);

    WSACleanup();
}

void EchoUDPClientLayer::OnAttach()
{
    initialize();
}

void EchoUDPClientLayer::OnDetach()
{
    // 생성된 소켓 라이브러리 해제
    closesocket(hClntSock);

    WSACleanup();
}

void EchoUDPClientLayer::OnUpdate(Hazel::Timestep ts)
{
    Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.f});
    Hazel::RenderCommand::Clear();
}

void EchoUDPClientLayer::OnEvent(Hazel::Event &event)
{
}

void EchoUDPClientLayer::OnImGuiRender()
{
    ImGuiChatWindow();
}

void EchoUDPClientLayer::ImGuiChatWindow()
{
    ImGui::Begin("Chat", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    // Display chat history
    // if (ImGui::BeginChild("ChatHistory",
    //                       ImVec2(0, -ImGui::GetItemsViewRect().h / 2 + 60),
    //                       true))
    // {
    //     ImGui::TextWrapped(chatHistory.begin(), chatHistory.end());
    //     if (recvBufferSize > 0)
    //     {
    //         chatHistory.appendf("\nReceived: %s\n", recvBuffer);
    //         recvBufferSize = 0; // Clear receive buffer after displaying
    //     }
    //     ImGui::SetScrollHereY(1.0f); // Always scroll to the bottom
    // }
    // ImGui::EndChild();

    // Input field for message
    // ImGui::InputTextMultiline("",
    //                             messageBuffer,
    //                             256,
    //                             ImVec2(-1.0f, 60.0f),
    //                             ImGuiInputTextFlags_EnterReturnsTrue);

    char messageBuffer[256];
    memset(messageBuffer, 0, sizeof(messageBuffer));
    strcpy_s(messageBuffer, sizeof(messageBuffer), m_InputText.c_str());

    if (ImGui::InputText("##Name", messageBuffer, sizeof(messageBuffer)))
    {
        m_InputText = messageBuffer;
    }

    // Send button
    if (ImGui::Button("Send", ImVec2(100, 0)))
    {
        if (m_InputText.length() > 0)
        {
            int readLen = 0;
            int readCnt = 0;

            // send, recv       : connected 소켓을 이용한 데이터 송수신에 사용됨
            // sendto, recvto : unconnected 소켓을 이용한 데이터 송수신에 사용됨
            send(hClntSock,
                          m_InputText.c_str(),
                          strlen(m_InputText.c_str()),
                          0);

            readLen =
                recv(hClntSock, messageBuffer, sizeof(messageBuffer) - 1, 0);

            messageBuffer[readLen] = 0;

            printf("Message from UDP server : %s \n", messageBuffer);
        }
    }

    ImGui::End();
}

void EchoUDPClientLayer::initialize()
{
    SOCKADDR_IN servAddr;

    // 소켓 라이브러리 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        NetworkUtil::ErrorHandling("WSAStartUp() Error");

    // UDP 소켓
    hClntSock = socket(PF_INET, SOCK_DGRAM, 0);

    // 소켓 생성
    if (hClntSock == INVALID_SOCKET)
        NetworkUtil::ErrorHandling("socket() Error");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(TEST_SERVER_IP_ADDRESS);
    servAddr.sin_port = htons(atoi(TEST_SERVER_PORT));

    // UDP 소켓에 대해 connect 함수 호출 --> connected 소켓으로 만들기
    connect(hClntSock, (SOCKADDR *)&servAddr, sizeof(servAddr));
}
