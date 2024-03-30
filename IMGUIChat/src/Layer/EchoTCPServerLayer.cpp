/*
Iterative Echo ����
1. ������ �� ������ �ϳ��� Ŭ���̾�Ʈ�� ����Ǿ� ���� ���� ����
2. ������ �� 5���� Ŭ���̾�Ʈ���� ���������� ���� �����ϰ� ����
3. Ŭ���̾�Ʈ�� ���α׷� ����ڷκ��� ���ڿ� �����͸� �Է� �޾Ƽ� ������ ����
4. ������ ���� ���� ���ڿ� �����͸� Ŭ���̾�Ʈ���� ������. ��, ���� ��Ų��.
5. ������ Ŭ���̾�Ʈ���� ���ڿ� ���ڴ� Ŭ���̾�Ʈ�� Q�� �Է��� ������ ���

*/

#include "EchoTCPServerLayer.h"
#include "ServerInfo.h"
#include "Util/Util.h"

class EchoTCPClientApp : public Hazel::Application
{
public:
    EchoTCPClientApp(const Hazel::ApplicationSpecification &specification)
        : Hazel::Application(specification)
    {
        // PushLayer(new ChatServerLayer());
        PushLayer(new EchoTCPServerLayer());
    }

    ~EchoTCPClientApp()
    {
    }
};

Hazel::Application *Hazel::CreateApplication(
    Hazel::ApplicationCommandLineArgs args)
{
    Hazel::ApplicationSpecification spec;
    spec.Name = "Sandbox";
    spec.WorkingDirectory = "IMGUIChat";
    spec.CommandLineArgs = args;

    return new EchoTCPClientApp(spec);
}

void EchoTCPServerLayer::OnAttach()
{
    initializeConnection();
}

void EchoTCPServerLayer::OnDetach()
{
    for (int i = 0; i < 5; ++i)
    {
        closesocket(hClntSock[i]);
    }
    closesocket(hServSock);
    WSACleanup(); // ���� ���̺귯�� ����
}

void EchoTCPServerLayer::OnUpdate(Hazel::Timestep ts)
{
}

void EchoTCPServerLayer::OnEvent(Hazel::Event &event)
{
}

void EchoTCPServerLayer::OnImGuiRender()
{
    ImGuiChatWindow();
}

void EchoTCPServerLayer::ImGuiChatWindow()
{
    ImGui::Begin("Chat");

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

void EchoTCPServerLayer::ImGuiConnectWindow()
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

void EchoTCPServerLayer::ImGuiCreateClientWindow()
{
    ImGui::Begin("Chat");

    // Send button
    if (ImGui::Button("CreateClient", ImVec2(100, 0)))
    {
        createClient();
        acceptConnection();
    }

    ImGui::End();
}

void EchoTCPServerLayer::createClient()
{
}

void EchoTCPServerLayer::initializeConnection()
{
    int szClntAddr;
    char message[] = "Hello world!";

    // if (argc != 2)
    // {
    //     printf("Usage : %s <port> \n", argv[0]);
    //     exit(1);
    // }

    // ���� ���̺귯�� �ʱ�ȭ
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        NetworkUtil::ErrorHandling("WSAStartUp() Error");

    // TCP ���� ����
    hServSock = socket(
        PF_INET, // domain : ������ ����� �������� ü��(Protocol Family) ���� ���� (IPv4 : PF_INET)
        SOCK_STREAM, // type : ������ ������ ���� ��Ŀ� ���� ���� ���� (TCP : SOCK_STREAM)
        0 // protocol : �� ��ǻ�Ͱ� ��ſ� ���Ǵ� �������� ���� ����
    );

    // ���� ����
    if (hServSock == INVALID_SOCKET)
        NetworkUtil::ErrorHandling("socket() Error");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET; // �ּ� ü�� ���� (IPv4  : 4����Ʈ �ּ�ü��)
    //  servAddr.sin_addr.s_addr = inet_addr(TEST_SERVER_IP_ADDRESS); // ���ڿ� -> ��Ʈ��ũ ����Ʈ ������ ��ȯ�� �ּ�
    servAddr.sin_addr.s_addr =
        htonl(INADDR_ANY); // ���ڿ� -> ��Ʈ��ũ ����Ʈ ������ ��ȯ�� �ּ�
    servAddr.sin_port =
        htons(atoi(TEST_SERVER_PORT)); // ���ڿ� ��� PORT ��ȣ ����

    // IP �ּ�, PORT ��ȣ �Ҵ� ���� (��, ���Ͽ� �ּ� �Ҵ�)
    if (bind(
            hServSock, // �ּ����� (IP, PORT)�� �Ҵ��� ������ ���� ��ũ����
            (SOCKADDR
                 *)&servAddr, // �Ҵ��ϰ��� �ϴ� �ּ������� ���ϴ�, ����ü ������ �ּҰ�
            sizeof(servAddr)) // 2��° ���� ���� ����
        == SOCKET_ERROR)
    {
        NetworkUtil::ErrorHandling("bind Error");
    }

    // ���� ��û ���� ���·� �����
    if (listen(hServSock, 5) == SOCKET_ERROR) // ���� ��û ���� ���·� �����
    {
        NetworkUtil::ErrorHandling("listen Error");
    }

    // Ŭ���̾�Ʈ ���� ��û �����ϱ�
    szClntAddr = sizeof(clntAddr);

}

void EchoTCPServerLayer::acceptConnection()
{
    // for (int i = 0; i < 5; ++i)
    for (int i = 0; i < 1; ++i)
    {
        hClntSock[i] = accept(hServSock, (SOCKADDR *)&clntAddr, &szClntAddr);

        if (hClntSock[i] == -1)
            NetworkUtil::ErrorHandling("accept() error");
        else
            printf("Connected client %d\n", i + 1);

        while (true)
        {
            hClntStrLen[i] = recv(hClntSock[i], message, BUF_SIZE, 0);

            if (hClntStrLen[i] == 0)
                continue;

            // printf("Message From Client %s\n", message);

            send(hClntSock[i], message, hClntStrLen[i], 0);

            break;
        }

        closesocket(hClntSock[i]);
    }
}
