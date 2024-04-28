#include "OverlappedServerLayer.h"
#include "Hazel/Core/Application/Application.h"
#include "Hazel/Core/ID/UUID.h"
#include "ServerInfo.h"
#include "Util/Util.h"

OverlappedServerLayer::~OverlappedServerLayer()
{
    
}

void OverlappedServerLayer::OnAttach()
{
    initialize();

    m_AcceptThread.SetThreadName(const_cast<char *>("AcceptThread"));
    m_AcceptThread.StartThread([&]() { this->receiveConnection(); });

    Hazel::ThreadUtils::InitSpinLock(&m_SpinLock);
}

void OverlappedServerLayer::OnDetach()
{
    if (m_CricSect)
    {
        delete m_CricSect;
        m_CricSect = nullptr;
    }

    for (ClientInfo* client : m_ConnectedClients)
    {
        // join �ϱ� 

        closesocket(client->clientSock);
        WSACloseEvent(client->evObj);
        delete client;
    };

    m_ConnectedClients.clear();
    
	closesocket(m_ReceiverSock);
    WSACleanup();
}

void OverlappedServerLayer::OnUpdate(Hazel::Timestep ts)
{
    Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.f});
    Hazel::RenderCommand::Clear();
}

void OverlappedServerLayer::OnEvent(Hazel::Event &event)
{
}

void OverlappedServerLayer::OnImGuiRender()
{
}

void OverlappedServerLayer::ImGuiChatWindow()
{
}

void OverlappedServerLayer::initialize()
{ 
    // ���� ����
    // Overlapped IO�� ������ ��-���ŷ ����� ������ �����ȴ�.
    // hListenSock = socket(PF_INET, SOCK_STREAM, 0);
    m_ReceiverSock =
        WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    
	memset(&m_ServAddr, 0, sizeof(m_ServAddr));
    m_ServAddr.sin_family = AF_INET;
    m_ServAddr.sin_addr.s_addr = inet_addr(TEST_SERVER_IP_ADDRESS);
    m_ServAddr.sin_port = htons(atoi(TEST_SERVER_PORT));

	// IP �ּ�, PORT ��ȣ �Ҵ�
    if (bind(m_ReceiverSock, (SOCKADDR*)&m_ServAddr, sizeof(m_ServAddr)) ==
        SOCKET_ERROR)
    {
        NetworkUtil::ErrorHandling("bind Error");
    }

    // ���� ��û ���� ���� -> ���� ��û ��� ť : 5�� ��û ��� ����
    if (listen(m_ReceiverSock, 5) == SOCKET_ERROR)
    {
        NetworkUtil::ErrorHandling("listen Error");
    }
    
}

void OverlappedServerLayer::receiveMessage(Hazel::UUID threadId)
{
    // SRWLock �� �̿��ϱ�
    // ���� ����..?

    // �ش� �Լ� ȣ�� ���Ŀ���, �������� ������ ��ӵȴٸ�
    if (WSARecv(hRecvSock,
                &dataBuf,
                1,
                (LPDWORD)&recvBytes,
                (LPDWORD)&flags,
                &overlapped,
                NULL) == SOCKET_ERROR)
    {
        // �ش� �Լ� ȣ�� ���Ŀ���, �������� ������ ��ӵȴٸ�
        if (WSAGetLastError() == WSA_IO_PENDING)
        {
            puts("Background data receive");

            // �ش� IO�� ������ overlapped.hEvent, �� evObj �̺�Ʈ Ŀ�� ������Ʈ�� signaled ���°� �ȴ�
            // signaled ���°� �� ������ ��ٸ���. (WSA_INFINITE)
            WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);

            // ���� ���۵� �������� ũ�⸦ Ȯ��
            WSAGetOverlappedResult(hRecvSock,
                                   &overlapped,
                                   (LPDWORD)&recvBytes,
                                   FALSE,
                                   NULL);
        }
        else
        {
            ErrorHandling("WSARecv() Error");
        }
    }
}

void OverlappedServerLayer::receiveConnection()
{
    while (true)
    {
        SOCKADDR_IN recvAdr;

        // ���� ��û�� �����Ǵ� ���� ����
        int recvAdrSize = sizeof(recvAdr);

        SOCKET newClientSock =
            accept(m_ReceiverSock, (SOCKADDR *)&recvAdr, &recvAdrSize);

        ClientInfo* newClientInfo = new ClientInfo();
        newClientInfo->clientSock = newClientSock;

        // manual-reset, non-signaled ������ �̺�Ʈ ����
        newClientInfo->evObj = WSACreateEvent();

        // ��� ��Ʈ 0���� �ʱ�ȭ
        memset(&newClientInfo->overlapped, 0, sizeof(newClientInfo->overlapped));
        newClientInfo->overlapped.hEvent = newClientInfo->evObj;
        newClientInfo->dataBuf.len = BUF_SIZE;
        newClientInfo->dataBuf.buf = newClientInfo->buf;

        std::string threadID = std::to_string(newClientInfo->id.Get());

        newClientInfo->communicateThread.SetThreadName(
            const_cast<char *>(threadID.c_str())
        );
        
        newClientInfo->communicateThread.StartThread(
            [&]() { this->receiveMessage(); }
        );

        Hazel::ThreadUtils::LockSpinLock(&m_SpinLock);

        m_ConnectedClients[newClientInfo->id] = newClientInfo;

        Hazel::ThreadUtils::UnlockSpinLock(&m_SpinLock);
    }
}
