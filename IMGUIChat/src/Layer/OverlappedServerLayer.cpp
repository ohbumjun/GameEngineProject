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
    for (auto &clientInfo : m_ConnectedClients)
    {
        clientInfo.second->communicateThread.JoinThread();
        closesocket(clientInfo.second->clientSock);
        WSACloseEvent(clientInfo.second->evObj);
        delete clientInfo.second;
    };

    m_ConnectedClients.clear();

    CloseHandle(hJob);
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
    TempIMGUIUtils::PrepareDockSpace();
    ImGuiChatWindow();
    ImGuiCreateClientWindow();
}

void OverlappedServerLayer::ImGuiChatWindow()
{
    // 여러 client 와 연결될 수 있다.
    // 현재 click 한 client 의 메세지만 window 에 띄울 것이다.
}

void OverlappedServerLayer::ImGuiCreateClientWindow()
{
    ImGui::Begin("CreateClient");

    // Send button
    if (ImGui::Button("CreateClient", ImVec2(100, 0)))
    {
        createClient();
    }

    ImGui::End();
}

void OverlappedServerLayer::createClient()
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    const Hazel::ApplicationSpecification &specification =
        Hazel::Application::Get().GetSpecification();

    std::string strExecutablePath = specification.CommandLineArgs[0];

    char commandLine[256]; // Allocate more space

    char numAddedToPort[2];
    sprintf(numAddedToPort, "%d", (int)m_ClientCount++);

    sprintf(commandLine,
            "%s OVERLAPPED_SENDER %s",
            strExecutablePath.c_str(),
            numAddedToPort); // Format the command line string

    // Start the child process.
    if (!CreateProcess(
            NULL,        // No module name (use command line)
            commandLine, // Command line
            NULL,        // Process handle not inheritable
            NULL,        // Thread handle not inheritable
            FALSE,       // Set handle inheritance to FALSE
            // 새 프로세스의 기본 스레드는 일시 중단된 상태로 만들어지고
            // ResumeThread 함수가 호출될 때까지 실행되지 않습니다.
            CREATE_SUSPENDED,
            NULL, // Use parent's environment block
            NULL, // Use parent's starting directory
            &si,  // Pointer to STARTUPINFO structure
            &pi)  // Pointer to PROCESS_INFORMATION structure
    )
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return;
    }

    if (!AssignProcessToJobObject(hJob, pi.hProcess))
    {
        printf("AssignProcessToJobObject failed (%d).\n", GetLastError());

        CloseHandle(hJob);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return;
    }

    // Resume the child process.
    if (ResumeThread(pi.hThread) == -1)
    {
        printf("ResumeThread failed (%d).\n", GetLastError());
        return;
    }

    // handle, thread handle 닫기
    // 사실상 ref count -1 시켜주기
    // 그러면 부모 프로세스 입장에서, 자식 프로세스에 대해 가지고 있는 Ref Cnt 를
    // -1 시켜준다.
    // 이를 통해, 자식 프로세스가 죽으면, 자식 커널 오브젝트가 사라지게 한다.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void OverlappedServerLayer::createJobObject()
{
    /*
    * Job 이란 ?
    * https://www.youtube.com/watch?v=2QRkNCrBrjI
    * 
    * Job Object 란, 1개 이상의 프로세스들을 하나의 set 로 관리하는
    * 커널 오브젝트이다.
    * 
    * 
    */
    hJob = CreateJobObject(NULL, NULL);

    if (hJob == NULL)
    {
        printf("CreateJobObject failed (%d).\n", GetLastError());
        CloseHandle(hJob);
        return;
    }

    // Set up job information to kill all child processes when the job is closed.
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = {0};
    jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if (!SetInformationJobObject(hJob,
                                 JobObjectExtendedLimitInformation,
                                 &jeli,
                                 sizeof(jeli)))
    {
        printf("SetInformationJobObject failed (%d).\n", GetLastError());
        return;
    }
}

void OverlappedServerLayer::createConnection()
{
    // 소켓 라이브러리 초기화
    if (WSAStartup(MAKEWORD(2, 2), &m_WsaData) != 0)
        NetworkUtil::ErrorHandling("WSAStartUp() Error");

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
    if (bind(m_ReceiverSock, (SOCKADDR *)&m_ServAddr, sizeof(m_ServAddr)) ==
        SOCKET_ERROR)
    {
        NetworkUtil::ErrorHandling("bind Error");
    }

    // 연결 요청 수락 상태 -> 연결 요청 대기 큐 : 10개 요청 대기 가능
    if (listen(m_ReceiverSock, 10) == SOCKET_ERROR)
    {
        NetworkUtil::ErrorHandling("listen Error");
    }
}

void OverlappedServerLayer::initialize()
{
	createConnection();
    createJobObject();
}

void OverlappedServerLayer::receiveMessage(Hazel::UUID threadId)
{
    // SRWLock 을 이용하기
    // 직접 구현..?
    int recvBytes = 0, flags = 0;
    
    Hazel::ThreadUtils::LockSpinLock(&m_SpinLock);

    ClientInfo *clientInfo = m_ConnectedClients[threadId];

    Hazel::ThreadUtils::UnlockSpinLock(&m_SpinLock);

    // 해당 함수 호출 이후에도, 데이터의 수신이 계속된다면
    if (WSARecv(clientInfo->clientSock,
                &clientInfo->dataBuf,
                1,
                (LPDWORD)&recvBytes,
                (LPDWORD)&flags,
                &clientInfo->overlapped,
                NULL) == SOCKET_ERROR)
    {
        // 해당 함수 호출 이후에도, 데이터의 수신이 계속된다면
        if (WSAGetLastError() == WSA_IO_PENDING)
        {
            // 해당 IO가 끝나면 overlapped.hEvent, 
            // 즉 evObj 이벤트 커널 오브젝트가 signaled 상태가 된다
            // signaled 상태가 될 때까지 기다린다. (WSA_INFINITE)
            WSAWaitForMultipleEvents(1,
                                     &clientInfo->evObj,
                                     TRUE,
                                     WSA_INFINITE,
                                     FALSE);

            // 실제 전송된 데이터의 크기를 확인
            WSAGetOverlappedResult(clientInfo->clientSock,
                                   &clientInfo->overlapped,
                                   (LPDWORD)&recvBytes,
                                   FALSE,
                                   NULL);

            std::cout << "Received Message From Client : " << clientInfo->buf << std::endl;

            Hazel::ThreadUtils::LockSpinLock(&clientInfo->clientDataLock);

            clientInfo->sentMessage.push_back(clientInfo->buf);

            Hazel::ThreadUtils::LockSpinLock(&clientInfo->clientDataLock);
        }
        else
        {
            NetworkUtil::ErrorHandling("WSARecv() Error");
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
        newClientInfo->idString = std::to_string(newClientInfo->id.Get());
        newClientInfo->communicateThread.SetThreadName(
            const_cast<char *>(newClientInfo->idString.c_str())
        );
        
        newClientInfo->communicateThread.StartThread(
            [&]() { this->receiveMessage(newClientInfo->id); }
        );

        Hazel::ThreadUtils::LockSpinLock(&m_SpinLock);

        m_ConnectedClients[newClientInfo->id] = newClientInfo;

        Hazel::ThreadUtils::UnlockSpinLock(&m_SpinLock);
    }
}
