/*
Iterative Echo ����
1. ������ �� ������ �ϳ��� Ŭ���̾�Ʈ�� ����Ǿ� ���� ���� ����
2. ������ �� 5���� Ŭ���̾�Ʈ���� ���������� ���� �����ϰ� ����
3. Ŭ���̾�Ʈ�� ���α׷� ����ڷκ��� ���ڿ� �����͸� �Է� �޾Ƽ� ������ ����
4. ������ ���� ���� ���ڿ� �����͸� Ŭ���̾�Ʈ���� ������. ��, ���� ��Ų��.
5. ������ Ŭ���̾�Ʈ���� ���ڿ� ���ڴ� Ŭ���̾�Ʈ�� Q�� �Է��� ������ ���

*/

#include "EchoUDPServerLayer.h"
#include "Hazel/Core/Application/Application.h"
#include "ServerInfo.h"
#include "Util/Util.h"

class EchoTCPClientApp : public Hazel::Application
{
public:
    EchoTCPClientApp(const Hazel::ApplicationSpecification &specification)
        : Hazel::Application(specification)
    {
        // PushLayer(new ChatServerLayer());
        PushLayer(new EchoUDPServerLayer());
    }

    ~EchoTCPClientApp()
    {
    }
};


EchoUDPServerLayer::~EchoUDPServerLayer()
{
    for (const auto &piInfo : m_Pids)
    {
        const PROCESS_INFORMATION &pi = piInfo.second;

        // Close process and thread handles.
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    closesocket(m_InitServSock);
    WSACleanup(); // ���� ���̺귯�� ����
}

void EchoUDPServerLayer::OnAttach()
{
    initializeConnection();
}

void EchoUDPServerLayer::OnDetach()
{
    closesocket(m_InitServSock);
    WSACleanup(); // ���� ���̺귯�� ����
}

void EchoUDPServerLayer::OnUpdate(Hazel::Timestep ts)
{
    Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.f});
    Hazel::RenderCommand::Clear();
}

void EchoUDPServerLayer::OnEvent(Hazel::Event &event)
{
}

void EchoUDPServerLayer::OnImGuiRender()
{
    TempIMGUIUtils::PrepareDockSpace();
    ImGuiChatWindow();
    ImGuiCreateClientWindow();
}

void EchoUDPServerLayer::ImGuiChatWindow()
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
        if (strlen(m_MessageBuffer) > 0)
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

void EchoUDPServerLayer::ImGuiConnectWindow()
{
    ImGui::Begin("Connect", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Username:");
    ImGui::InputText("", m_Username, 32);

    // Connect button
    if (ImGui::Button("Connect", ImVec2(100, 0)))
    {
        if (strlen(m_Username) > 0)
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

void EchoUDPServerLayer::ImGuiCreateClientWindow()
{
    ImGui::Begin("CreateClient");

    // Send button
    if (ImGui::Button("CreateClient", ImVec2(100, 0)))
    {
        createClient();
        acceptConnection();
    }

    ImGui::End();
}

void EchoUDPServerLayer::createClient()
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    const Hazel::ApplicationSpecification &specification =
        Hazel::Application::Get().GetSpecification();

    const char *constCharExecutablePath = specification.CommandLineArgs[0];

    char commandLine[256]; // Allocate more space

    sprintf(commandLine,
            "%s ECO_UDP_CLIENT",
            constCharExecutablePath); // Format the command line string

    // Start the child process.
    if (!CreateProcess(NULL,        // No module name (use command line)
                       commandLine, // Command line
                       NULL,        // Process handle not inheritable
                       NULL,        // Thread handle not inheritable
                       FALSE,       // Set handle inheritance to FALSE
                       0,           // No creation flags
                       NULL,        // Use parent's environment block
                       NULL,        // Use parent's starting directory
                       &si,         // Pointer to STARTUPINFO structure
                       &pi)         // Pointer to PROCESS_INFORMATION structure
    )
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return;
    }

    m_Pids.insert({pi.dwProcessId, pi});
}

void EchoUDPServerLayer::initializeConnection()
{
    // ���� ���̺귯�� �ʱ�ȭ
    if (WSAStartup(MAKEWORD(2, 2), &m_WsaData) != 0)
        NetworkUtil::ErrorHandling("WSAStartUp() Error");

    // UDP ���� ����
    m_InitServSock = socket(
        PF_INET, // domain : ������ ����� �������� ü��(Protocol Family) ���� ���� (IPv4 : PF_INET)
        SOCK_DGRAM, // type : ������ ������ ���� ��Ŀ� ���� ���� ���� (TCP : SOCK_STREAM)
        0 // protocol : �� ��ǻ�Ͱ� ��ſ� ���Ǵ� �������� ���� ����
    );

    // ���� ����
    if (m_InitServSock == INVALID_SOCKET)
        NetworkUtil::ErrorHandling("socket() Error");

    memset(&m_ServAddr, 0, sizeof(m_ServAddr));
    m_ServAddr.sin_family =
        AF_INET; // �ּ� ü�� ���� (IPv4  : 4����Ʈ �ּ�ü��)
    //  servAddr.sin_addr.s_addr = inet_addr(TEST_SERVER_IP_ADDRESS); // ���ڿ� -> ��Ʈ��ũ ����Ʈ ������ ��ȯ�� �ּ�
    m_ServAddr.sin_addr.s_addr =
        htonl(INADDR_ANY); // ���ڿ� -> ��Ʈ��ũ ����Ʈ ������ ��ȯ�� �ּ�
    // inet_addr(
    //     TEST_SERVER_IP_ADDRESS); // ���ڿ� -> ��Ʈ��ũ ����Ʈ ������ ��ȯ�� �ּ�
    m_ServAddr.sin_port =
        htons(atoi(TEST_SERVER_PORT)); // ���ڿ� ��� PORT ��ȣ ����

    // IP �ּ�, PORT ��ȣ �Ҵ� ���� (��, ���Ͽ� �ּ� �Ҵ�)
    if (bind(
            m_InitServSock, // �ּ����� (IP, PORT)�� �Ҵ��� ������ ���� ��ũ����
            (SOCKADDR
                 *)&m_ServAddr, // �Ҵ��ϰ��� �ϴ� �ּ������� ���ϴ�, ����ü ������ �ּҰ�
            sizeof(m_ServAddr)) // 2��° ���� ���� ����
        == SOCKET_ERROR)
    {
        NetworkUtil::ErrorHandling("bind Error");
    }

    // UDP �����̹Ƿ� listen �Լ��� ������ �ʿ����� �ʴ�.
    // 1:1 �� �̸� ������ �س��� ���� �ƴϱ� �����̴�.
    m_ClntAddrSize = sizeof(m_ClntAddr);
}

void EchoUDPServerLayer::acceptConnection()
{
    while (1)
    {
        // for (int i = 0; i < 5; ++i)
        for (int i = 0; i < 1; ++i)
        {
            int strLen = 0;

            // recvFrom, sendTo �Լ� -> unconnected ������ �̿��� ����, ���� �Լ�
            // �Ҵ�� �ּҷ� ���޵Ǵ� ��� ������ ����
            strLen = recvfrom(m_InitServSock,
                              m_RecvBuffer,
                              BUF_SIZE,
                              0,
                              (SOCKADDR *)&m_ClntAddr,
                              &m_ClntAddrSize);
            m_RecvBuffer[0] = 0;

            m_RecvBuffer[strLen] = 0; // ������ NULL ���� ����]

            printf("Message From UDP Client : %s", m_RecvBuffer);

            sendto(m_InitServSock,
                   m_RecvBuffer,
                   strLen,
                   0,
                   (SOCKADDR *)&m_ClntAddr,
                   sizeof(m_ClntAddrSize));
	

        }
    }
}
