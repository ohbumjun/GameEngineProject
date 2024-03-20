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

// 네트워크 바이트 순서로 변환해주는 함수
// unsigned short htons(unsigned short);
// unsigned short ntohs(unsigned short);
// unsigned long htonl(unsigned long);
// unsigned long ntohl(unsigned long);
// short 형 데이터를,
//    호스트 바이트 순서에서 네트워크 바이트 순서로 변환해라.
//
//    short 형 데이터를,
//    네트워크 바이트 순서에서 호스트 바이트 순서 로 변환해라.
//
//    long 형 데이터를,
//    호스트 바이트 순서에서 네트워크 바이트 순서로 변환해라.
//
//    long 형 데이터를,
//    네트워크 바이트 순서에서 호스트 바이트 순서 로 변환해라.

void ChatServerLayer::OnAttach()
{
    int szClntAddr;
    char message[] = "Hello world!";

    char *serv_ip = "211.217.168.13"; // IP 주소 문자열 지정
    char *serv_port = "9190";         // PORT 번호 문자열 지정

    // if (argc != 2)
    // {
    //     printf("Usage : %s <port> \n", argv[0]);
    //     exit(1);
    // }

    // 소켓 라이브러리 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartUp() Error");

    // TCP 소켓 생성
    hServSock = socket(
        PF_INET,      // domain : 소켓이 사용할 프로토콜 체계(Protocol Family) 정보 전달 (IPv4 : PF_INET)
        SOCK_STREAM,  // type : 소켓의 데이터 전송 방식에 대한 정보 전달 (TCP : SOCK_STREAM)
        0             // protocol : 두 컴퓨터간 통신에 사용되는 프로토콜 정보 전달
    );

    // 소켓 생성
    if (hServSock == INVALID_SOCKET)
        ErrorHandling("socket() Error");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;                   // 주소 체계 지정 (IPv4  : 4바이트 주소체계)

    // inet_addr : 문자열 ip 주소를 네트워크 바이트 순서로 정렬된 32 비트 정수로 표현
    servAddr.sin_addr.s_addr = inet_addr(serv_ip);   // 문자열 -> 네트워크 바이트 순서로 변환한 주소
    // servAddr.sin_port = htons(atoi(argv[1]));
    servAddr.sin_port = htons(atoi(serv_port));      // 문자열 기반 PORT 번호 지정

    // IP 주소, PORT 번호 할당 목적 (즉, 소켓에 주소 할당)
    if (bind(
        hServSock,              // 주소정보 (IP, PORT)를 할당할 소켓의 파일 디스크립터
        (SOCKADDR*)&servAddr,   // 할당하고자 하는 주소정보를 지니는, 구조체 변수의 주소값
        sizeof(servAddr))       // 2번째 인자 길이 정보
        == SOCKET_ERROR)
    {
        ErrorHandling("bind Error");
    }

    if (listen(hServSock, 5) == SOCKET_ERROR)   // 연결 요청 수락 상태로 만들기
    {
        ErrorHandling("listen Error");
    }

    szClntAddr = sizeof(clntAddr);
   
    hClntSock = accept(hServSock, (SOCKADDR *)&clntAddr, &szClntAddr); // 클라이언트 프로그램에서의 연결 요청 수락

    if (hClntSock == INVALID_SOCKET)
    {
        ErrorHandling("accept() Error");
    }
   
    send(hClntSock, message, sizeof(message), 0); // 연결된 클라리언트에 데이터 전송
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
