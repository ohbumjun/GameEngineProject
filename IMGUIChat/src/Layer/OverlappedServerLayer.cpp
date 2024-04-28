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
        // join 하기 

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
    // 서버 소켓
    // Overlapped IO가 가능한 넌-블로킹 모드의 소켓이 생성된다.
    // hListenSock = socket(PF_INET, SOCK_STREAM, 0);
    m_ReceiverSock =
        WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    
	memset(&m_ServAddr, 0, sizeof(m_ServAddr));
    m_ServAddr.sin_family = AF_INET;
    m_ServAddr.sin_addr.s_addr = inet_addr(TEST_SERVER_IP_ADDRESS);
    m_ServAddr.sin_port = htons(atoi(TEST_SERVER_PORT));

	// IP 주소, PORT 번호 할당
    if (bind(m_ReceiverSock, (SOCKADDR*)&m_ServAddr, sizeof(m_ServAddr)) ==
        SOCKET_ERROR)
    {
        NetworkUtil::ErrorHandling("bind Error");
    }

    // 연결 요청 수락 상태 -> 연결 요청 대기 큐 : 5개 요청 대기 가능
    if (listen(m_ReceiverSock, 5) == SOCKET_ERROR)
    {
        NetworkUtil::ErrorHandling("listen Error");
    }
    
}

void OverlappedServerLayer::receiveMessage(Hazel::UUID threadId)
{
    // SRWLock 을 이용하기
    // 직접 구현..?

    // 해당 함수 호출 이후에도, 데이터의 수신이 계속된다면
    if (WSARecv(hRecvSock,
                &dataBuf,
                1,
                (LPDWORD)&recvBytes,
                (LPDWORD)&flags,
                &overlapped,
                NULL) == SOCKET_ERROR)
    {
        // 해당 함수 호출 이후에도, 데이터의 수신이 계속된다면
        if (WSAGetLastError() == WSA_IO_PENDING)
        {
            puts("Background data receive");

            // 해당 IO가 끝나면 overlapped.hEvent, 즉 evObj 이벤트 커널 오브젝트가 signaled 상태가 된다
            // signaled 상태가 될 때까지 기다린다. (WSA_INFINITE)
            WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);

            // 실제 전송된 데이터의 크기를 확인
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

        // 연결 요청에 대응되는 소켓 생성
        int recvAdrSize = sizeof(recvAdr);

        SOCKET newClientSock =
            accept(m_ReceiverSock, (SOCKADDR *)&recvAdr, &recvAdrSize);

        ClientInfo* newClientInfo = new ClientInfo();
        newClientInfo->clientSock = newClientSock;

        // manual-reset, non-signaled 형태의 이벤트 생성
        newClientInfo->evObj = WSACreateEvent();

        // 모든 비트 0으로 초기화
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
