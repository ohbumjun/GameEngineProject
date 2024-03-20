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

// ��Ʈ��ũ ����Ʈ ������ ��ȯ���ִ� �Լ�
// unsigned short htons(unsigned short);
// unsigned short ntohs(unsigned short);
// unsigned long htonl(unsigned long);
// unsigned long ntohl(unsigned long);
// short �� �����͸�,
//    ȣ��Ʈ ����Ʈ �������� ��Ʈ��ũ ����Ʈ ������ ��ȯ�ض�.
//
//    short �� �����͸�,
//    ��Ʈ��ũ ����Ʈ �������� ȣ��Ʈ ����Ʈ ���� �� ��ȯ�ض�.
//
//    long �� �����͸�,
//    ȣ��Ʈ ����Ʈ �������� ��Ʈ��ũ ����Ʈ ������ ��ȯ�ض�.
//
//    long �� �����͸�,
//    ��Ʈ��ũ ����Ʈ �������� ȣ��Ʈ ����Ʈ ���� �� ��ȯ�ض�.

void ChatServerLayer::OnAttach()
{
    int szClntAddr;
    char message[] = "Hello world!";

    char *serv_ip = "211.217.168.13"; // IP �ּ� ���ڿ� ����
    char *serv_port = "9190";         // PORT ��ȣ ���ڿ� ����

    // if (argc != 2)
    // {
    //     printf("Usage : %s <port> \n", argv[0]);
    //     exit(1);
    // }

    // ���� ���̺귯�� �ʱ�ȭ
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartUp() Error");

    // TCP ���� ����
    hServSock = socket(
        PF_INET,      // domain : ������ ����� �������� ü��(Protocol Family) ���� ���� (IPv4 : PF_INET)
        SOCK_STREAM,  // type : ������ ������ ���� ��Ŀ� ���� ���� ���� (TCP : SOCK_STREAM)
        0             // protocol : �� ��ǻ�Ͱ� ��ſ� ���Ǵ� �������� ���� ����
    );

    // ���� ����
    if (hServSock == INVALID_SOCKET)
        ErrorHandling("socket() Error");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;                   // �ּ� ü�� ���� (IPv4  : 4����Ʈ �ּ�ü��)

    // inet_addr : ���ڿ� ip �ּҸ� ��Ʈ��ũ ����Ʈ ������ ���ĵ� 32 ��Ʈ ������ ǥ��
    servAddr.sin_addr.s_addr = inet_addr(serv_ip);   // ���ڿ� -> ��Ʈ��ũ ����Ʈ ������ ��ȯ�� �ּ�
    // servAddr.sin_port = htons(atoi(argv[1]));
    servAddr.sin_port = htons(atoi(serv_port));      // ���ڿ� ��� PORT ��ȣ ����

    // IP �ּ�, PORT ��ȣ �Ҵ� ���� (��, ���Ͽ� �ּ� �Ҵ�)
    if (bind(
        hServSock,              // �ּ����� (IP, PORT)�� �Ҵ��� ������ ���� ��ũ����
        (SOCKADDR*)&servAddr,   // �Ҵ��ϰ��� �ϴ� �ּ������� ���ϴ�, ����ü ������ �ּҰ�
        sizeof(servAddr))       // 2��° ���� ���� ����
        == SOCKET_ERROR)
    {
        ErrorHandling("bind Error");
    }

    if (listen(hServSock, 5) == SOCKET_ERROR)   // ���� ��û ���� ���·� �����
    {
        ErrorHandling("listen Error");
    }

    szClntAddr = sizeof(clntAddr);
   
    hClntSock = accept(hServSock, (SOCKADDR *)&clntAddr, &szClntAddr); // Ŭ���̾�Ʈ ���α׷������� ���� ��û ����

    if (hClntSock == INVALID_SOCKET)
    {
        ErrorHandling("accept() Error");
    }
   
    send(hClntSock, message, sizeof(message), 0); // ����� Ŭ�󸮾�Ʈ�� ������ ����
}

void ChatServerLayer::OnDetach()
{
    closesocket(hClntSock);
    closesocket(hServSock);
    WSACleanup(); // ���� ���̺귯�� ����
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
