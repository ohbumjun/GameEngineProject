/*
Iterative Echo 서버
1. 서버는 한 순간에 하나의 클라이언트와 연결되어 에코 서비스 제공
2. 서버는 총 5개의 클라이언트에게 순차적으로 서비스 제공하고 종료
3. 클라이언트는 프로그램 사용자로부터 문자열 데이터를 입력 받아서 서버에 전송
4. 서버는 전송 받은 문자열 데이터를 클라이언트에게 재전송. 즉, 에코 시킨다.
5. 서버와 클라이언트와의 문자열 에코는 클라이언트가 Q를 입력할 때까지 계속

*/

#include "EchoTCPServerLayer.h"
#include "ServerInfo.h"
#include "Hazel/Core/Application/Application.h"
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


EchoTCPServerLayer::~EchoTCPServerLayer()
{
    for (const auto &piInfo : m_Pids)
    {
        const PROCESS_INFORMATION& pi = piInfo.second;

        // Close process and thread handles.
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
	}
    for (int i = 0; i < 5; ++i)
    {
        closesocket(m_ClntSocks[i]);
    }
    closesocket(m_InitServSock);
    WSACleanup(); // 윈속 라이브러리 해제
}

void EchoTCPServerLayer::OnAttach()
{
    initializeConnection();
}

void EchoTCPServerLayer::OnDetach()
{
}

void EchoTCPServerLayer::OnUpdate(Hazel::Timestep ts)
{
    Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.f});
    Hazel::RenderCommand::Clear();
}

void EchoTCPServerLayer::OnEvent(Hazel::Event &event)
{
}

void EchoTCPServerLayer::OnImGuiRender()
{
    TempIMGUIUtils::PrepareDockSpace();
    ImGuiChatWindow();
    ImGuiCreateClientWindow();
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

void EchoTCPServerLayer::ImGuiConnectWindow()
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

void EchoTCPServerLayer::ImGuiCreateClientWindow()
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

void EchoTCPServerLayer::createClient()
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
            "%s ECO_TCP_CLIENT",
            constCharExecutablePath); // Format the command line string

    // Start the child process.
    if (!CreateProcess(NULL, // No module name (use command line)
                       commandLine, // Command line
                        NULL,  // Process handle not inheritable
                        NULL,  // Thread handle not inheritable
                        FALSE, // Set handle inheritance to FALSE
                        0,       // No creation flags
                        NULL,  // Use parent's environment block
                        NULL,  // Use parent's starting directory
                        &si,   // Pointer to STARTUPINFO structure
                        &pi)   // Pointer to PROCESS_INFORMATION structure
    )
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return;
    }

    m_Pids.insert({pi.dwProcessId, pi});
}

void EchoTCPServerLayer::initializeConnection()
{
    // 소켓 라이브러리 초기화
    if (WSAStartup(MAKEWORD(2, 2), &m_WsaData) != 0)
        NetworkUtil::ErrorHandling("WSAStartUp() Error");

    // TCP 소켓 생성
    m_InitServSock = socket(
        PF_INET, // domain : 소켓이 사용할 프로토콜 체계(Protocol Family) 정보 전달 (IPv4 : PF_INET)
        SOCK_STREAM, // type : 소켓의 데이터 전송 방식에 대한 정보 전달 (TCP : SOCK_STREAM)
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

    // 연결 요청 수락 상태로 만들기
    if (listen(m_InitServSock, 5) ==
        SOCKET_ERROR) // 연결 요청 수락 상태로 만들기
    {
        NetworkUtil::ErrorHandling("listen Error");
    }

    // 클라이언트 연결 요청 수락하기
    m_ClntAddrSize = sizeof(m_ClntAddr);

}

void EchoTCPServerLayer::acceptConnection()
{
    // while (1)
    {
        // for (int i = 0; i < 5; ++i)
        for (int i = 0; i < 1; ++i)
        {
            // m_ClntSocks[i] = accept(m_InitServSock,
            SOCKET AcceptSocket = accept(m_InitServSock,
                                    (SOCKADDR *)&m_ClntAddr, 
                                    &m_ClntAddrSize
            );

            if (AcceptSocket == INVALID_SOCKET)
            {
                NetworkUtil::ErrorHandling("accept() error");
                closesocket(AcceptSocket);
                continue;
            }
            else
            {
                printf("Connected TCP client %d\n", i + 1);
            }

            while (true)
            {
                // Server 측에서도 단 한번만 recv 를 해도 되는 건가 ?
                m_ClntStrLen[i] =
                    // recv(m_ClntSocks[i], m_RecvBuffer, BUF_SIZE, 0);
                    recv(AcceptSocket, m_RecvBuffer, BUF_SIZE, 0);

                if (m_ClntStrLen[i] == -1)
                    continue;

                if (m_ClntStrLen[i] == 0)
                    continue;

                m_RecvBuffer[m_ClntStrLen[i]] = 0;

                printf("Message From TCP Client %s\n", m_RecvBuffer);
            
                // send(m_ClntSocks[i], m_RecvBuffer, m_ClntStrLen[i], 0);
                send(AcceptSocket, m_RecvBuffer, m_ClntStrLen[i], 0);
            
                break;
            }
            
            // closesocket(m_ClntSocks[i]);
            closesocket(AcceptSocket);
        }
    }
    
}
