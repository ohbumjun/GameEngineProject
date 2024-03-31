/*
Iterative Echo 서버
1. 서버는 한 순간에 하나의 클라이언트와 연결되어 에코 서비스 제공
2. 서버는 총 5개의 클라이언트에게 순차적으로 서비스 제공하고 종료
3. 클라이언트는 프로그램 사용자로부터 문자열 데이터를 입력 받아서 서버에 전송
4. 서버는 전송 받은 문자열 데이터를 클라이언트에게 재전송. 즉, 에코 시킨다.
5. 서버와 클라이언트와의 문자열 에코는 클라이언트가 Q를 입력할 때까지 계속

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
    WSACleanup(); // 윈속 라이브러리 해제
}

void EchoUDPServerLayer::OnAttach()
{
    initializeConnection();
}

void EchoUDPServerLayer::OnDetach()
{
    closesocket(m_InitServSock);
    WSACleanup(); // 윈속 라이브러리 해제
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
    // 소켓 라이브러리 초기화
    if (WSAStartup(MAKEWORD(2, 2), &m_WsaData) != 0)
        NetworkUtil::ErrorHandling("WSAStartUp() Error");

    // UDP 소켓 생성
    m_InitServSock = socket(
        PF_INET, // domain : 소켓이 사용할 프로토콜 체계(Protocol Family) 정보 전달 (IPv4 : PF_INET)
        SOCK_DGRAM, // type : 소켓의 데이터 전송 방식에 대한 정보 전달 (TCP : SOCK_STREAM)
        0 // protocol : 두 컴퓨터간 통신에 사용되는 프로토콜 정보 전달
    );

    // 소켓 생성
    if (m_InitServSock == INVALID_SOCKET)
        NetworkUtil::ErrorHandling("socket() Error");

    memset(&m_ServAddr, 0, sizeof(m_ServAddr));
    m_ServAddr.sin_family =
        AF_INET; // 주소 체계 지정 (IPv4  : 4바이트 주소체계)
    //  servAddr.sin_addr.s_addr = inet_addr(TEST_SERVER_IP_ADDRESS); // 문자열 -> 네트워크 바이트 순서로 변환한 주소
    m_ServAddr.sin_addr.s_addr =
        htonl(INADDR_ANY); // 문자열 -> 네트워크 바이트 순서로 변환한 주소
    // inet_addr(
    //     TEST_SERVER_IP_ADDRESS); // 문자열 -> 네트워크 바이트 순서로 변환한 주소
    m_ServAddr.sin_port =
        htons(atoi(TEST_SERVER_PORT)); // 문자열 기반 PORT 번호 지정

    // IP 주소, PORT 번호 할당 목적 (즉, 소켓에 주소 할당)
    if (bind(
            m_InitServSock, // 주소정보 (IP, PORT)를 할당할 소켓의 파일 디스크립터
            (SOCKADDR
                 *)&m_ServAddr, // 할당하고자 하는 주소정보를 지니는, 구조체 변수의 주소값
            sizeof(m_ServAddr)) // 2번째 인자 길이 정보
        == SOCKET_ERROR)
    {
        NetworkUtil::ErrorHandling("bind Error");
    }

    // UDP 소켓이므로 listen 함수가 별도로 필요하지 않다.
    // 1:1 로 미리 연결을 해놓는 것이 아니기 때문이다.
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

            // recvFrom, sendTo 함수 -> unconnected 소켓을 이용한 전송, 수신 함수
            // 할당된 주소로 전달되는 모든 데이터 수신
            strLen = recvfrom(m_InitServSock,
                              m_RecvBuffer,
                              BUF_SIZE,
                              0,
                              (SOCKADDR *)&m_ClntAddr,
                              &m_ClntAddrSize);
            m_RecvBuffer[0] = 0;

            m_RecvBuffer[strLen] = 0; // 마지막 NULL 문자 삽입]

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
