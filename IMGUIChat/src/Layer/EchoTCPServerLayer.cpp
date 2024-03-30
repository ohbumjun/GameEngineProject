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
    WSACleanup(); // 윈속 라이브러리 해제
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

    // 소켓 라이브러리 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        NetworkUtil::ErrorHandling("WSAStartUp() Error");

    // TCP 소켓 생성
    hServSock = socket(
        PF_INET, // domain : 소켓이 사용할 프로토콜 체계(Protocol Family) 정보 전달 (IPv4 : PF_INET)
        SOCK_STREAM, // type : 소켓의 데이터 전송 방식에 대한 정보 전달 (TCP : SOCK_STREAM)
        0 // protocol : 두 컴퓨터간 통신에 사용되는 프로토콜 정보 전달
    );

    // 소켓 생성
    if (hServSock == INVALID_SOCKET)
        NetworkUtil::ErrorHandling("socket() Error");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET; // 주소 체계 지정 (IPv4  : 4바이트 주소체계)
    //  servAddr.sin_addr.s_addr = inet_addr(TEST_SERVER_IP_ADDRESS); // 문자열 -> 네트워크 바이트 순서로 변환한 주소
    servAddr.sin_addr.s_addr =
        htonl(INADDR_ANY); // 문자열 -> 네트워크 바이트 순서로 변환한 주소
    servAddr.sin_port =
        htons(atoi(TEST_SERVER_PORT)); // 문자열 기반 PORT 번호 지정

    // IP 주소, PORT 번호 할당 목적 (즉, 소켓에 주소 할당)
    if (bind(
            hServSock, // 주소정보 (IP, PORT)를 할당할 소켓의 파일 디스크립터
            (SOCKADDR
                 *)&servAddr, // 할당하고자 하는 주소정보를 지니는, 구조체 변수의 주소값
            sizeof(servAddr)) // 2번째 인자 길이 정보
        == SOCKET_ERROR)
    {
        NetworkUtil::ErrorHandling("bind Error");
    }

    // 연결 요청 수락 상태로 만들기
    if (listen(hServSock, 5) == SOCKET_ERROR) // 연결 요청 수락 상태로 만들기
    {
        NetworkUtil::ErrorHandling("listen Error");
    }

    // 클라이언트 연결 요청 수락하기
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
