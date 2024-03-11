#include <backends/imgui_impl_glfw.h>
#include <imgui.h>
#include <stdio.h>
// #include <winsock2.h> // For Windows (replace with appropriate headers for other platforms)
#include "ChatLayer.h"
// #include <arpa/inet.h> // For Linux/macOS (uncomment if needed)

// #pragma comment(lib, "ws2_32.lib") // For Windows (link with Winsock library)

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

void IMGUIChatLayer::ImGuiChatWindow()
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

void IMGUIChatLayer::ImGuiConnectWindow()
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
