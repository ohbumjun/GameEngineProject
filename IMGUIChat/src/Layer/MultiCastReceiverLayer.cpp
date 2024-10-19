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
    m_ReceiveThread.StartThread([&]() { this->receiveResponse(); });

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
        ImGui::Text("%s", m_ReceivedMessage[i].c_str());
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
        PF_INET,             // domain : ������ ����� �������� ü��(Protocol Family) ���� ���� (IPv4 : PF_INET)
        SOCK_DGRAM,     // type : ������ ������ ���� ��Ŀ� ���� ���� ���� (TCP : SOCK_STREAM)
        IPPROTO_UDP // protocol : �� ��ǻ�Ͱ� ��ſ� ���Ǵ� �������� ���� ����
    );

    // ���� ����
    if (m_ReceiverSock == INVALID_SOCKET)
    {
        NetworkUtil::ErrorHandling("socket() Error");
    }

    memset(&m_ServAddr, 0, sizeof(m_ServAddr));
    
    m_ServAddr.sin_family = AF_INET; // �ּ� ü�� ���� (IPv4  : 4����Ʈ �ּ�ü��)
    
    // Receiver �������� � address �� ����Ѵ�.
    m_ServAddr.sin_addr.s_addr = htonl(INADDR_ANY); // ���ڿ� -> ��Ʈ��ũ ����Ʈ ������ ��ȯ�� �ּ�
    
    // port ��ȣ�� sender �ʿ��� ������ port ��ȣ�� �ؾ� �Ѵ�.
    m_ServAddr.sin_port = htons(atoi(TEST_SERVER_PORT));

   //- On Windows, binding a UDP socket to a port also implicitly joins the multicast group 
   //  associated with that port (if applicable).
    if (bind(m_ReceiverSock, (SOCKADDR*)&m_ServAddr, sizeof(m_ServAddr)) ==
        SOCKET_ERROR)
    {
        int bindError = WSAGetLastError();
        std::cerr << "Bind failed with error code: " << bindError << " - ";
        char *errorMessage;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                          FORMAT_MESSAGE_FROM_SYSTEM |
                          FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL,
                      bindError,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                      (LPSTR)&errorMessage,
                      0,
                      NULL);
        std::cerr << errorMessage << std::endl;
        LocalFree(errorMessage);
        closesocket(m_ReceiverSock);
        WSACleanup();

        NetworkUtil::ErrorHandling("bind Error");
        return;
    }

    joinMulticastGroup();
}

void MultiCastReceiverLayer::receiveResponse()
{
    static char recvBuffer[1024];

    char ReceivedIP[46] = {0};

    while (1)
    {
        memset(recvBuffer, 0, BUF_SIZE - 1);

        // recvfrom : unconnected ������ �̿��� ������ �ۼ���
        // ���� �� ���� ���ϴµ� �� �Լ��� �񵿱�� �����ϸ� �Ƿ���..?
        receiveLen = recvfrom(m_ReceiverSock, 
            recvBuffer, 
            BUF_SIZE - 1, 
            0, 
            NULL, 
            0);

        if (receiveLen < 0)
            break;

        recvBuffer[receiveLen] = 0;

        printf("Message from MT Sender : %s \n", recvBuffer);

        sockaddr_in ClientAddr;
        inet_ntop(AF_INET, &ClientAddr.sin_addr, (PSTR)ReceivedIP, 46);
        std::cout << "Received from: " << ReceivedIP << ", "
                  << ntohs(ClientAddr.sin_port) << "\n";
  
        Hazel::ThreadUtils::LockCritSect(m_CricSect);

        m_ReceivedMessage.push_back(recvBuffer);
        
        Hazel::ThreadUtils::UnlockCritSect(m_CricSect);
    }
}

void MultiCastReceiverLayer::joinMulticastGroup()
{
    // Ip is the multicast IP we want to receive from
    std::string IP = TEST_MULTICAST_IP_ADDRESS;
    if (inet_pton(AF_INET,
                  (PCSTR)(IP.c_str()),
                  &m_JoinAdr.imr_multiaddr.s_addr) <
        0)
    {
        std::cout << "IP invalid\n";
        closesocket(m_ReceiverSock);
        WSACleanup();
        return;
    }
   
    // �׷쿡 ������ ȣ��Ʈ�� �ּ�����
    m_JoinAdr.imr_interface.s_addr = htonl(INADDR_ANY);

    if (setsockopt(m_ReceiverSock,
                IPPROTO_IP,                  // �׷찡�Կ� ���Ǵ� �������� ����
                IP_ADD_MEMBERSHIP,   // �ش� �������� �������� ���Ǵ� �������� �̸�
                (char *)&m_JoinAdr,
                sizeof(m_JoinAdr)) == SOCKET_ERROR)
    {
        NetworkUtil::ErrorHandling("setsock() error");
    }
}
