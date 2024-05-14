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
    // ���� client �� ����� �� �ִ�.
    // ���� click �� client �� �޼����� window �� ��� ���̴�.
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
            // �� ���μ����� �⺻ ������� �Ͻ� �ߴܵ� ���·� ���������
            // ResumeThread �Լ��� ȣ��� ������ ������� �ʽ��ϴ�.
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

    // handle, thread handle �ݱ�
    // ��ǻ� ref count -1 �����ֱ�
    // �׷��� �θ� ���μ��� ���忡��, �ڽ� ���μ����� ���� ������ �ִ� Ref Cnt ��
    // -1 �����ش�.
    // �̸� ����, �ڽ� ���μ����� ������, �ڽ� Ŀ�� ������Ʈ�� ������� �Ѵ�.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void OverlappedServerLayer::createJobObject()
{
    /*
    * Job �̶� ?
    * https://www.youtube.com/watch?v=2QRkNCrBrjI
    * 
    * Job Object ��, 1�� �̻��� ���μ������� �ϳ��� set �� �����ϴ�
    * Ŀ�� ������Ʈ�̴�.
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
    // ���� ���̺귯�� �ʱ�ȭ
    if (WSAStartup(MAKEWORD(2, 2), &m_WsaData) != 0)
        NetworkUtil::ErrorHandling("WSAStartUp() Error");

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
    if (bind(m_ReceiverSock, (SOCKADDR *)&m_ServAddr, sizeof(m_ServAddr)) ==
        SOCKET_ERROR)
    {
        NetworkUtil::ErrorHandling("bind Error");
    }

    // ���� ��û ���� ���� -> ���� ��û ��� ť : 10�� ��û ��� ����
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
    // SRWLock �� �̿��ϱ�
    // ���� ����..?
    int recvBytes = 0, flags = 0;
    
    Hazel::ThreadUtils::LockSpinLock(&m_SpinLock);

    ClientInfo *clientInfo = m_ConnectedClients[threadId];

    Hazel::ThreadUtils::UnlockSpinLock(&m_SpinLock);

    // �ش� �Լ� ȣ�� ���Ŀ���, �������� ������ ��ӵȴٸ�
    if (WSARecv(clientInfo->clientSock,
                &clientInfo->dataBuf,
                1,
                (LPDWORD)&recvBytes,
                (LPDWORD)&flags,
                &clientInfo->overlapped,
                NULL) == SOCKET_ERROR)
    {
        // �ش� �Լ� ȣ�� ���Ŀ���, �������� ������ ��ӵȴٸ�
        if (WSAGetLastError() == WSA_IO_PENDING)
        {
            // �ش� IO�� ������ overlapped.hEvent, 
            // �� evObj �̺�Ʈ Ŀ�� ������Ʈ�� signaled ���°� �ȴ�
            // signaled ���°� �� ������ ��ٸ���. (WSA_INFINITE)
            WSAWaitForMultipleEvents(1,
                                     &clientInfo->evObj,
                                     TRUE,
                                     WSA_INFINITE,
                                     FALSE);

            // ���� ���۵� �������� ũ�⸦ Ȯ��
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
