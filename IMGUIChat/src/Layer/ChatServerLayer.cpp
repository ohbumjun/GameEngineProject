#include <backends/imgui_impl_glfw.h>
#include <imgui.h>
#include <stdio.h>
#include "ChatServerLayer.h"

// Network-related variables
#define SERVER_PORT 12345
#define SERVER_ADDRESS                                                         \
    "127.0.0.1" // Replace with actual server IP if connecting remotely
// SOCKET sock;
// struct sockaddr_in serverAddr;
bool connected = false;
char recvBuffer[1024];
int recvBufferSize = 0;


// ImGui-related variables
ImGuiTextBuffer chatHistory;
bool showConnectWindow = true;
char username[32] = "";
char messageBuffer[256] = "";


void ErrorHandling(const char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void ChatServerLayer::OnAttach()
{
    int szClntAddr;
    char message[] = "Hello world!";

    if (argc != 2)
    {
        printf("Usage : %s <port> \n", argv[0]);
        exit(1);
    }

    // 소켓 라이브러리 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartUp() Error");

    // TCP 소켓 생성
    hServSock = socket(
        PF_INET,            // domain : 소켓이 사용할 프로토콜 체계(Protocol Family) 정보 전달
        SOCK_STREAM,  // type : 소켓의 데이터 전송 방식에 대한 정보 전달
        0                       // protocol : 두 컴퓨터간 통신에 사용되는 프로토콜 정보 전달
    );

    // 소켓 생성
    if (hServSock == INVALID_SOCKET)
        ErrorHandling("socket() Error");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));

    // IP 주소, PORT 번호 할당 목적 (즉, 소켓에 주소 할당)
    if (bind(hServSock, (SOCKADDR *)&servAddr, sizeof(servAddr)) ==
        SOCKET_ERROR)
        ErrorHandling("bind Error");

    // 연결 요청 수락 상태로 만들기
    if (listen(hServSock, 5) == SOCKET_ERROR)   // 연결 요청 수락 상태로 만들기
        ErrorHandling("listen Error");

    szClntAddr = sizeof(clntAddr);

    // 클라이언트 프로그램에서의 연결 요청 수락
    hClntSock = accept(hServSock, (SOCKADDR *)&clntAddr, &szClntAddr);

    if (hClntSock == INVALID_SOCKET)
        ErrorHandling("accept() Error");

    // 연결된 클라리언트에 데이터 전송
    send(hClntSock, message, sizeof(message), 0);

}

void ChatServerLayer::OnDetach()
{
    closesocket(hClntSock);
    closesocket(hServSock);
    WSACleanup(); // 윈속 라이브러리 해제
}

void ChatServerLayer::OnUpdate(Hazel::Timestep ts)
{
}

void ChatServerLayer::OnEvent(Hazel::Event &event)
{
}

void ChatServerLayer::ImGuiChatWindow()
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

    // Send button
    if (ImGui::Button("Send", ImVec2(100, 0)))
    {
        if (strlen(messageBuffer) > 0)
        {
            // Send message to server
           // send(sock, messageBuffer, strlen(messageBuffer), 0);
           // memset(
           //     messageBuffer,
           //     0,
           //     sizeof(
           //         messageBuffer)); // Clear message buffer after sending
        }
    }

    ImGui::End();
}

void ChatServerLayer::ImGuiConnectWindow()
{
    ImGui::Begin("Connect", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Username:");
    ImGui::InputText("", username, 32);

    // Connect button
    if (ImGui::Button("Connect", ImVec2(100, 0)))
    {
        if (strlen(username) > 0)
        {
            // Initialize Winsock (Windows-specific)
            // WSADATA wsaData;
            // if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            // {
            //     printf("WSAStartup failed with error: %d\n", WSAGetLastError());
            //     return;
            // }
            //
            // // Create socket
            // sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            // if (sock == INVALID_SOCKET)
            // {
            //     printf("socket creation failed with error: %d\n",
            //            WSAGetLastError());
            //     return;
            // }
            //
            // // Setup server address
            // serverAddr.sin_family = AF_INET;
            // serverAddr.sin_port = htons(SERVER_PORT);
            // // inet_pton(AF_INET, SERVER_ADDRESS, &serverAddr.sin_addr);
            //
            // // Connect to server
            // if (connect(sock,
            //             (struct sockaddr *)&serverAddr,
            //             sizeof(serverAddr)) == SOCKET_ERROR)
            // {
            //     printf("connect failed with error: %d\n", WSAGetLastError());
            //     closesocket(sock);
            //     return;
            // }
            //
            // connected = true;
            // showConnectWindow = false;
        }
    }

    ImGui::End();
}
