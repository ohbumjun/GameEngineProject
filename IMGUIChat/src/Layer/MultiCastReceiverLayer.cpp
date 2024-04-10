/*
Iterative Echo 서버
1. 서버는 한 순간에 하나의 클라이언트와 연결되어 에코 서비스 제공
2. 서버는 총 5개의 클라이언트에게 순차적으로 서비스 제공하고 종료
3. 클라이언트는 프로그램 사용자로부터 문자열 데이터를 입력 받아서 서버에 전송
4. 서버는 전송 받은 문자열 데이터를 클라이언트에게 재전송. 즉, 에코 시킨다.
5. 서버와 클라이언트와의 문자열 에코는 클라이언트가 Q를 입력할 때까지 계속

*/

#include "MultiCastReceiverLayer.h"
#include "Hazel/Core/Application/Application.h"
#include "ServerInfo.h"
#include "Util/Util.h"

MultiCastReceiverLayer::~MultiCastReceiverLayer()
{
    closesocket(m_ReceiverSock);
    WSACleanup(); // 윈속 라이브러리 해제
}

void MultiCastReceiverLayer::OnAttach()
{
    initializeConnection();

    m_ReceiveThread.SetThreadName(const_cast<char *>("ReceiveThread"));

    // Thread 가, Worker의 Execute 함수를 실행할 수 있게 한다.
    m_ReceiveThread.StartThread(&MultiCastReceiverLayer::receiveResponse, this);

    m_CricSect = Hazel::ThreadUtils::CreateCritSect();
}

void MultiCastReceiverLayer::OnDetach()
{
}

void MultiCastReceiverLayer::OnUpdate(Hazel::Timestep ts)
{
    Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.f});
    Hazel::RenderCommand::Clear();
}

void MultiCastReceiverLayer::OnEvent(Hazel::Event &event)
{
}
 
void MultiCastReceiverLayer::OnImGuiRender()
{
    TempIMGUIUtils::PrepareDockSpace();
    ImGuiChatWindow();
}

void MultiCastReceiverLayer::ImGuiChatWindow()
{
    static bool scrollToBottom = false;

    ImGui::Begin("Chat");
    
    ImGui::BeginChild("Received",
                      ImVec2(0, -ImGui::GetFrameHeightWithSpacing()),
                      true);

    Hazel::ThreadUtils::LockCritSect(m_CricSect);

    for (int i = 0; i < m_ReceivedMessage.size(); i++)
    {
        ImGui::Text("%s", m_ReceivedMessage[i]);
    }

    Hazel::ThreadUtils::UnlockCritSect(m_CricSect);

    if (scrollToBottom)
    {
        ImGui::SetScrollHereY(1.0f);
    }
    
    scrollToBottom = false;

    ImGui::EndChild();

    ImGui::Separator();

    // Send button
    if (ImGui::Button("Send", ImVec2(100, 0)))
    {
    }

    ImGui::End();
}

void MultiCastReceiverLayer::initializeConnection()
{
    // 소켓 라이브러리 초기화
    if (WSAStartup(MAKEWORD(2, 2), &m_WsaData) != 0)
        NetworkUtil::ErrorHandling("WSAStartUp() Error");

    // UDP 소켓 생성
    m_ReceiverSock = socket(
        PF_INET, // domain : 소켓이 사용할 프로토콜 체계(Protocol Family) 정보 전달 (IPv4 : PF_INET)
        SOCK_DGRAM, // type : 소켓의 데이터 전송 방식에 대한 정보 전달 (TCP : SOCK_STREAM)
        0 // protocol : 두 컴퓨터간 통신에 사용되는 프로토콜 정보 전달
    );

    // 소켓 생성
    if (m_ReceiverSock == INVALID_SOCKET)
        NetworkUtil::ErrorHandling("socket() Error");

    memset(&m_ServAddr, 0, sizeof(m_ServAddr));

    m_ServAddr.sin_family = AF_INET; // 주소 체계 지정 (IPv4  : 4바이트 주소체계)
    m_ServAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 문자열 -> 네트워크 바이트 순서로 변환한 주소

    int portNum = atoi(TEST_SERVER_PORT);
    
    const Hazel::ApplicationSpecification &specification = Hazel::Application::Get().GetSpecification();

    if (specification.CommandLineArgs.GetCount() > 2)
    {
        std::string addedPortNum = specification.CommandLineArgs[2];
        portNum += atoi(addedPortNum.c_str());
    }

    m_ServAddr.sin_port = htons(portNum); // 문자열 기반 PORT 번호 지정

    // The bind function for UDP sockets behaves differently on Windows compared to Linux/Unix systems. 
    // On Windows, binding a UDP socket to a port also implicitly joins the multicast group 
    // associated with that port (if applicable).
    if (bind(m_ReceiverSock, (SOCKADDR*)&m_ServAddr, sizeof(m_ServAddr)) ==
        SOCKET_ERROR)
    {
        NetworkUtil::ErrorHandling("bind Error");
        return;
    }

    // 데이터를 수신하는 측에서는 가입이라는 절차를 추가적으로 거쳐야 한다.
	m_JoinAdr.imr_multiaddr.s_addr = inet_addr(TEST_MULTICAST_IP_ADDRESS);
    m_JoinAdr.imr_interface.s_addr = htonl(INADDR_ANY);
  	
    // setsockopt() 함수를 이용하여 소켓 옵션 설정
    // IP_ADD_MEMBERSHIP : 멀티캐스트 그룹에 가입
    if (setsockopt(m_ReceiverSock,
        IPPROTO_IP,
        IP_ADD_MEMBERSHIP,
        (const char*)&m_JoinAdr,
        sizeof(m_JoinAdr)) == SOCKET_ERROR)
    {
        NetworkUtil:: ErrorHandling("setsock() error");
    }
}

void MultiCastReceiverLayer::receiveResponse()
{
    static char recvBuffer[1024];

    while (1)
    {
        memset(recvBuffer, 0, BUF_SIZE - 1);

        // 현재 잘 동작 안하는데 이 함수를 비동기로 변경하면 되려나..?
        receiveLen = recvfrom(m_ReceiverSock, 
            recvBuffer, BUF_SIZE - 1, 0, NULL, 0);

        if (receiveLen < 0)
            break;

        recvBuffer[receiveLen] = 0;

        printf("Message from MT Sender : %s \n", recvBuffer);

        Hazel::ThreadUtils::LockCritSect(m_CricSect);

        m_ReceivedMessage.emplace_back(recvBuffer);
        
        Hazel::ThreadUtils::UnlockCritSect(m_CricSect);
    }
}
