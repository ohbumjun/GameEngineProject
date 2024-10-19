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
        /*
        *   Parent Process �� ���Ƿ� �ݰ� �Ǹ�, Job Object �� ��ϵ� ��� Child Process ����
        *   ���� ����ȴ�.
        */
        // clientInfo.second->communicateThread.JoinThread();
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
    ImGui::Begin("Chat", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    char messageBuffer[256];
    memset(messageBuffer, 0, sizeof(messageBuffer));
    strcpy_s(messageBuffer, sizeof(messageBuffer), m_InputText.c_str());

    if (ImGui::InputText("##Name", messageBuffer, sizeof(messageBuffer)))
    {
        m_InputText = messageBuffer;
    }

    // Send button
    if (ImGui::Button("Send", ImVec2(100, 0)))
    {
        if (m_InputText.length() > 0)
        {
            int readLen = 0;
            int readCnt = 0;

            char buf[BUF_SIZE];

            memcpy(buf, m_InputText.c_str(), m_InputText.length() + 1);

            // send, recv       : connected ������ �̿��� ������ �ۼ��ſ� ����
            // sendto, recvFrom : unconnected ������ �̿��� ������ �ۼ��ſ� ����
            //. sendto(hSenderSock,
            //.        // m_InputText.c_str(),
            //.        //  strlen(m_InputText.c_str()),
            //.        buf,
            //.        strlen(buf),
            //.        0,
            //.        (SOCKADDR *)&senderAddr,
            //.        sizeof(senderAddr));
        }
    }

    Hazel::ThreadUtils::LockSpinLock(&m_SpinLock);

     if (ImGui::BeginCombo("Projection", "Current Chatting Client"))
    {
        for (auto &clientInfo : m_ConnectedClients)
        {
            bool isSelected = m_CurrentChatUUID == clientInfo.first;

            if (ImGui::Selectable(clientInfo.second->idString.c_str(), isSelected))
            {
                m_CurrentChatUUID = clientInfo.first;
			}

            if (isSelected)
            {
				ImGui::SetItemDefaultFocus();
			}
        };

        ImGui::EndCombo();
    };

     if (m_ConnectedClients.empty())
    {
        Hazel::ThreadUtils::UnlockSpinLock(&m_SpinLock);
        ImGui::End();
        return;
     }

     ClientInfo* currentChatClient = m_ConnectedClients[m_CurrentChatUUID];


    for (int i = 0; i < currentChatClient->receivedMessage.size(); i++)
    {
        ImGui::Text("Receive %s", currentChatClient->receivedMessage[i].c_str());
    }

    Hazel::ThreadUtils::UnlockSpinLock(&m_SpinLock);

    ImGui::End();
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
    if (listen(m_ReceiverSock, 5) == SOCKET_ERROR)
    {
        NetworkUtil::ErrorHandling("listen Error");
    }
    
	// manual reset + non-signaled ������ Event ������Ʈ ����
    m_ServerEvent = WSACreateEvent();

    // ���ϰ� �̺�Ʈ ������Ʈ ����
    // ��, ���Ͽ� ���ؼ� �����û�� ���� ���, newEvent �� Signaled ���·� �ٲ�� �ȴ�.
    // hServSock ���Ͽ��� 3��° ���ڷ� ���޵� �̺�Ʈ �� �ϳ��� �߻��ϸ�, newEvent �� Ŀ�ο�����Ʈ�� signaled ���·� �ٲ۴�.
    // 3��° ���ڷ� ���ް����� �̺�Ʈ ����
    // FD_ACCEPT, FD_READ, FD_WRITE, FD_OOB, FD_CONNECT, FD_CLOSE, FD_QOS ���
    // ���� �ϳ���, �ѹ��� �Լ� ȣ���� ����ȴ�. selec �Լ��� �޸� �� �Լ� ȣ�⶧���� ������ �� �ʿ䰡 ����.
    if (WSAEventSelect(m_ReceiverSock, m_ServerEvent, FD_ACCEPT) ==
        SOCKET_ERROR)
    {
        NetworkUtil::ErrorHandling("WSAEventSelect() Error");
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
    WSABUF dataBuf;
    char buf[BUF_SIZE];
    // ���ŵ� ������ ������ ����� ������ ������ ���� WSABUF ����ü
    dataBuf.len = BUF_SIZE;
    dataBuf.buf = buf;

    Hazel::ThreadUtils::LockSpinLock(&m_SpinLock);

    ClientInfo *clientInfo = m_ConnectedClients[threadId];

    Hazel::ThreadUtils::UnlockSpinLock(&m_SpinLock);

    SOCKET& clientSock = clientInfo->clientSock;

    WSAOVERLAPPED& overlapped = clientInfo->overlapped;

    // �ش� �Լ� ȣ�� ���Ŀ���, �������� ������ ��ӵȴٸ�
    if (WSARecv(clientSock,
                &dataBuf,
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
            // ex) Client ������ �۽��� ������, ���� Signaled ���±� �ȴٴ� ���̴�.
            DWORD waitResult = WSAWaitForMultipleEvents(1,
                                     &clientInfo->evObj,
                                     TRUE,
                                     WSA_INFINITE,
                                     FALSE);

           // ���⿡ �ɸ��ٴ� ����, �ش� �̺�Ʈ ������Ʈ�� sinaled���°��� ������Ʈ�� �ƴ϶�� �ǹ��̴�.
            if ((waitResult == WSA_WAIT_FAILED ||
                waitResult == WSA_WAIT_TIMEOUT))
            {
                return;
            }
            else
            {
                BOOL result = WSAGetOverlappedResult(clientInfo->clientSock,
                                                     &clientInfo->overlapped,
                                                     (LPDWORD)&recvBytes,
                                                     FALSE,
                                                     NULL);

                if (!result)
                {
                   
                }
                else
                {
                    Hazel::ThreadUtils::LockSpinLock(&clientInfo->clientDataLock);
                    clientInfo->receivedMessage.push_back(buf);
                    Hazel::ThreadUtils::UnlockSpinLock(&clientInfo->clientDataLock);
                }
            }
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
        newClientInfo->idString = std::to_string(newClientInfo->id.Get());
        newClientInfo->communicateThread.SetThreadName(
            const_cast<char *>(newClientInfo->idString.c_str())
        );

        Hazel::ThreadUtils::LockSpinLock(&m_SpinLock);

        m_ConnectedClients[newClientInfo->id] = newClientInfo;

        Hazel::ThreadUtils::UnlockSpinLock(&m_SpinLock);

        newClientInfo->communicateThread.StartThread(
            [&]() { this->receiveMessage(newClientInfo->id); }
        );

        m_CurrentChatUUID = newClientInfo->id;  

    }
}
