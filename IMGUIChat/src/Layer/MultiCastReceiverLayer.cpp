/*
Iterative Echo ����
1. ������ �� ������ �ϳ��� Ŭ���̾�Ʈ�� ����Ǿ� ���� ���� ����
2. ������ �� 5���� Ŭ���̾�Ʈ���� ���������� ���� �����ϰ� ����
3. Ŭ���̾�Ʈ�� ���α׷� ����ڷκ��� ���ڿ� �����͸� �Է� �޾Ƽ� ������ ����
4. ������ ���� ���� ���ڿ� �����͸� Ŭ���̾�Ʈ���� ������. ��, ���� ��Ų��.
5. ������ Ŭ���̾�Ʈ���� ���ڿ� ���ڴ� Ŭ���̾�Ʈ�� Q�� �Է��� ������ ���

*/

#include "MultiCastReceiverLayer.h"
#include "Hazel/Core/Application/Application.h"
#include "ServerInfo.h"
#include "Util/Util.h"

MultiCastReceiverLayer::~MultiCastReceiverLayer()
{
    closesocket(m_ReceiverSock);
    WSACleanup(); // ���� ���̺귯�� ����
}

void MultiCastReceiverLayer::OnAttach()
{
    initializeConnection();

    m_ReceiveThread.SetThreadName(const_cast<char *>("ReceiveThread"));

    // Thread ��, Worker�� Execute �Լ��� ������ �� �ְ� �Ѵ�.
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
    // ���� ���̺귯�� �ʱ�ȭ
    if (WSAStartup(MAKEWORD(2, 2), &m_WsaData) != 0)
        NetworkUtil::ErrorHandling("WSAStartUp() Error");

    // UDP ���� ����
    m_ReceiverSock = socket(
        PF_INET, // domain : ������ ����� �������� ü��(Protocol Family) ���� ���� (IPv4 : PF_INET)
        SOCK_DGRAM, // type : ������ ������ ���� ��Ŀ� ���� ���� ���� (TCP : SOCK_STREAM)
        0 // protocol : �� ��ǻ�Ͱ� ��ſ� ���Ǵ� �������� ���� ����
    );

    // ���� ����
    if (m_ReceiverSock == INVALID_SOCKET)
        NetworkUtil::ErrorHandling("socket() Error");

    memset(&m_ServAddr, 0, sizeof(m_ServAddr));

    m_ServAddr.sin_family = AF_INET; // �ּ� ü�� ���� (IPv4  : 4����Ʈ �ּ�ü��)
    m_ServAddr.sin_addr.s_addr = htonl(INADDR_ANY); // ���ڿ� -> ��Ʈ��ũ ����Ʈ ������ ��ȯ�� �ּ�

    int portNum = atoi(TEST_SERVER_PORT);
    
    const Hazel::ApplicationSpecification &specification = Hazel::Application::Get().GetSpecification();

    if (specification.CommandLineArgs.GetCount() > 2)
    {
        std::string addedPortNum = specification.CommandLineArgs[2];
        portNum += atoi(addedPortNum.c_str());
    }

    m_ServAddr.sin_port = htons(portNum); // ���ڿ� ��� PORT ��ȣ ����

    // The bind function for UDP sockets behaves differently on Windows compared to Linux/Unix systems. 
    // On Windows, binding a UDP socket to a port also implicitly joins the multicast group 
    // associated with that port (if applicable).
    if (bind(m_ReceiverSock, (SOCKADDR*)&m_ServAddr, sizeof(m_ServAddr)) ==
        SOCKET_ERROR)
    {
        NetworkUtil::ErrorHandling("bind Error");
        return;
    }

    // �����͸� �����ϴ� �������� �����̶�� ������ �߰������� ���ľ� �Ѵ�.
	m_JoinAdr.imr_multiaddr.s_addr = inet_addr(TEST_MULTICAST_IP_ADDRESS);
    m_JoinAdr.imr_interface.s_addr = htonl(INADDR_ANY);
  	
    // setsockopt() �Լ��� �̿��Ͽ� ���� �ɼ� ����
    // IP_ADD_MEMBERSHIP : ��Ƽĳ��Ʈ �׷쿡 ����
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

        // ���� �� ���� ���ϴµ� �� �Լ��� �񵿱�� �����ϸ� �Ƿ���..?
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
