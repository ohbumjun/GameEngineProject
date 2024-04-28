#include "MultiCastSenderLayer.h"
#include "Util/Util.h"

MultiCastSenderLayer::~MultiCastSenderLayer()
{
}

void MultiCastSenderLayer::OnAttach()
{
    initialize();

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

void MultiCastSenderLayer::OnDetach()
{
    finalize();
}

void MultiCastSenderLayer::OnUpdate(Hazel::Timestep ts)
{
    Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.f});
    Hazel::RenderCommand::Clear();
}

void MultiCastSenderLayer::OnEvent(Hazel::Event &event)
{
}

void MultiCastSenderLayer::OnImGuiRender()
{
    TempIMGUIUtils::PrepareDockSpace();
    ImGuiChatWindow();
    ImGuiCreateReceiverWindow();
}

void MultiCastSenderLayer::ImGuiChatWindow()
{
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
            sendto(hSenderSock,
                          // m_InputText.c_str(),
                         //  strlen(m_InputText.c_str()),
                        buf, 
                        strlen(buf),
                        0,
                        (SOCKADDR *)&senderAddr,
                        sizeof(senderAddr));
         }
    }

    ImGui::End();
}

void MultiCastSenderLayer::ImGuiCreateReceiverWindow()
{
    ImGui::Begin("CreateClient");

    // Send button
    if (ImGui::Button("Create MT Receiver", ImVec2(200, 0)))
    {
        createClient();
    }

    ImGui::End();
}

void MultiCastSenderLayer::initialize()
{
    // TTL : ��Ŷ�� �󸶳� �ָ����� ���ΰ�.
    // ��Ȯ����, �ִ� ��� ����͸� ���İ� �� �ְ� �� ���ΰ�.
    int timeLive = TTL;

    // ���� ���̺귯�� �ʱ�ȭ
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        NetworkUtil::ErrorHandling("WSAStartUp() Error");

    // UDP ����
    hSenderSock = socket(PF_INET, SOCK_DGRAM, 0);

    // ���� ����
    if (hSenderSock == INVALID_SOCKET)
    {
        NetworkUtil::ErrorHandling("socket() Error");
        WSACleanup();
        return;
    }

    // Allow reuse of port
    int optval = 1;
    if ((setsockopt(hSenderSock,
                    SOL_SOCKET,
                    SO_REUSEADDR,
                    (char *)&optval,
                    sizeof(optval))) < 0)
    {
        std::cout << "Socket set SO_REUSEADDR fail\n";
        closesocket(hSenderSock);
        WSACleanup();
        return;
    }

    /*
    This socket is almost identical to a unicast socket, the only difference is that you are sending to a multicast address. Namely, IP adress from 224.0.0.0 to 239.255.255.255.
    */
    // set taret address
    memset(&senderAddr, 0, sizeof(senderAddr));

    senderAddr.sin_family = AF_INET;

    std::string IP  = TEST_MULTICAST_IP_ADDRESS;

    // inet_pton : text ������ ipvt �ּҸ� binary ip newwork ���·� �������ִ� �Լ�
    if (inet_pton(AF_INET, (PCSTR)(IP.c_str()), &senderAddr.sin_addr.s_addr) <
        0)
    {
        std::cout << "Multicast failed set join group\n";
        closesocket(hSenderSock);
        WSACleanup();
        return;
    }

    senderAddr.sin_port = htons(atoi(TEST_SERVER_PORT));

    // ��Ƽ ĳ��Ʈ IP
    // senderAddr.sin_addr.s_addr = inet_addr(TEST_MULTICAST_IP_ADDRESS);
    // 
    // // ��Ƽ ĳ��Ʈ Port
    // senderAddr.sin_port             = htons(atoi(TEST_SERVER_PORT));
    // 

    // TTL ����
    setsockopt(hSenderSock,
               IPPROTO_IP,
               IP_MULTICAST_TTL,
               (const char*)(void *)&timeLive,
               sizeof(timeLive));
}

void MultiCastSenderLayer::finalize()
{
    CloseHandle(hJob);
    // ������ ���� ���̺귯�� ����
    closesocket(hSenderSock);
    WSACleanup();
}

void MultiCastSenderLayer::createClient()
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
            "%s MULTICAST_RECEIVER %s",
            strExecutablePath.c_str(),
            numAddedToPort); // Format the command line string

    // Start the child process.
    if (!CreateProcess(NULL,        // No module name (use command line)
                       commandLine, // Command line
                       NULL,        // Process handle not inheritable
                       NULL,        // Thread handle not inheritable
                       FALSE,       // Set handle inheritance to FALSE
                        // �� ���μ����� �⺻ ������� �Ͻ� �ߴܵ� ���·� ��������� 
                        // ResumeThread �Լ��� ȣ��� ������ ������� �ʽ��ϴ�.
                       CREATE_SUSPENDED, 
                       NULL,        // Use parent's environment block
                       NULL,        // Use parent's starting directory
                       &si,         // Pointer to STARTUPINFO structure
                       &pi)         // Pointer to PROCESS_INFORMATION structure
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

    // m_Pids.insert({pi.dwProcessId, pi});
}
