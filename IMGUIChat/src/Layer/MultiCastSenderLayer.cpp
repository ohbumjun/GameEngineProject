#include "MultiCastSenderLayer.h"
#include "Util/Util.h"

MultiCastSenderLayer::~MultiCastSenderLayer()
{
	finalize();
}

void MultiCastSenderLayer::OnAttach()
{
    initialize();
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

            // send, recv       : connected ������ �̿��� ������ �ۼ��ſ� ����
            // sendto, recvto : unconnected ������ �̿��� ������ �ۼ��ſ� ����
            sendto(hSenderSock,
                          m_InputText.c_str(),
                          strlen(m_InputText.c_str()),
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

    memset(&senderAddr, 0, sizeof(senderAddr));
    senderAddr.sin_family = AF_INET;
    // �߿��� ����, �ݵ�� IP �ּҸ� ��Ƽĳ��Ʈ �ּҷ� �����ؾ� �Ѵٴ� ���̴�.
    senderAddr.sin_addr.s_addr = inet_addr(TEST_MULTICAST_IP_ADDRESS);
    senderAddr.sin_port = htons(atoi(TEST_SERVER_PORT));
    
    // Set up for sending to multicast group (enable sending)
    setsockopt(hSenderSock,
               IPPROTO_IP,
               // The TTL determines the maximum number of routers a packet
               // can pass through before being discarded
               IP_MULTICAST_TTL,
               (const char *)(void *)&timeLive,
               sizeof(timeLive));
}

void MultiCastSenderLayer::finalize()
{
    for (const auto &piInfo : m_Pids)
    {
        const PROCESS_INFORMATION &pi = piInfo.second;

        // Close process and thread handles.
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

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

    const char *constCharExecutablePath = specification.CommandLineArgs[0];

    char commandLine[256]; // Allocate more space

    char numAddedToPort[2]; 
    sprintf(numAddedToPort, "%d", (int)m_Pids.size());

    sprintf(commandLine,
            "%s MULTICAST_RECEIVER %s",
            constCharExecutablePath,
            numAddedToPort); // Format the command line string

    // Start the child process.
    if (!CreateProcess(NULL,        // No module name (use command line)
                       commandLine, // Command line
                       NULL,        // Process handle not inheritable
                       NULL,        // Thread handle not inheritable
                       FALSE,       // Set handle inheritance to FALSE
                       0,           // No creation flags
                       NULL,        // Use parent's environment block
                       NULL,        // Use parent's starting directory
                       &si,         // Pointer to STARTUPINFO structure
                       &pi)         // Pointer to PROCESS_INFORMATION structure
    )
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return;
    }

    m_Pids.insert({pi.dwProcessId, pi});
}
