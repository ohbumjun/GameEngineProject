#include <backends/imgui_impl_glfw.h>
#include <imgui.h>
#include <stdio.h>
#include "ChatClientLayer.h"

#define SERVER_PORT "12345"
#define SERVER_IP_ADDRESS "127.0.0.1"
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


void ChatClientLayer::OnAttach()
{
    SOCKADDR_IN servAddr;

    char message[30];
    int strLen = 0, idx = 0, readLen = 0;

    if (argc != 3)
    {
        printf("Usage : %s <port> \n", argv[0]);
        exit(1);
    }

    // ���� ���̺귯�� �ʱ�ȭ
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartUp() Error");

    // TCP ����
    hClntSock = socket(PF_INET, SOCK_STREAM, 0);

    // ���� ����
    if (hClntSock == INVALID_SOCKET)
        ErrorHandling("socket() Error");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    servAddr.sin_port = htons(atoi(argv[1]));

    // ������ ������ �������� ������ ���� ��û
    if (connect(hClntSock, (SOCKADDR *)&servAddr, sizeof(servAddr)) ==
        SOCKET_ERROR)
        ErrorHandling("connect() Error");

    // �������� read �Լ� ȣ�� (�������� ��� ���� Ȯ���ϱ�)
    // ���ŵ� �����͸� 1 ����Ʈ�� �б�
    while (readLen = recv(hClntSock, &message[idx++], 1, 0))
    {
        if (readLen == -1)
            ErrorHandling("read() Error");

        strLen += readLen;
    }

    printf("Message from server : %s \n", message);

}

void ChatClientLayer::OnDetach()
{
    // ������ ���� ���̺귯�� ����
    closesocket(hClntSock);

    WSACleanup();
}

void ChatClientLayer::OnUpdate(Hazel::Timestep ts)
{
}

void ChatClientLayer::OnEvent(Hazel::Event &event)
{
}

void ChatClientLayer::ImGuiChatWindow()
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

void ChatClientLayer::ImGuiConnectWindow()
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
