#include "EchoTCPClientLayer.h"
#include "Util/Util.h"

EchoTCPClientLayer::~EchoTCPClientLayer()
{
	// ������ ���� ���̺귯�� ����
	closesocket(hClntSock);

	WSACleanup();
}

void EchoTCPClientLayer::OnAttach()
{
    connectServer();
}

void EchoTCPClientLayer::OnDetach()
{
    // ������ ���� ���̺귯�� ����
    closesocket(hClntSock);

    WSACleanup();
}

void EchoTCPClientLayer::OnUpdate(Hazel::Timestep ts)
{
}

void EchoTCPClientLayer::OnEvent(Hazel::Event &event)
{
}

void EchoTCPClientLayer::OnImGuiRender()
{
	ImGuiChatWindow();
}

void EchoTCPClientLayer::ImGuiChatWindow()
{
    ImGui::Begin("Chat", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    // Display chat history
    // if (ImGui::BeginChild("ChatHistory",
    //                       ImVec2(0, -ImGui::GetItemsViewRect().h / 2 + 60),
    //                       true))
    // {
    //     ImGui::TextWrapped(chatHistory.begin(), chatHistory.end());
    //     if (recvBufferSize > 0)
    //     {
    //         chatHistory.appendf("\nReceived: %s\n", recvBuffer);
    //         recvBufferSize = 0; // Clear receive buffer after displaying
    //     }
    //     ImGui::SetScrollHereY(1.0f); // Always scroll to the bottom
    // }
    // ImGui::EndChild();

    // Input field for message
    // ImGui::InputTextMultiline("",
    //                             messageBuffer,
    //                             256,
    //                             ImVec2(-1.0f, 60.0f),
    //                             ImGuiInputTextFlags_EnterReturnsTrue);

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
            int strLen      = 0;
            int readLen    = 0;
            int readCnt = 0;
            /*
		�ش� �ڵ��, read, write �Լ��� ȣ��� ������
		���ڿ� ������, ���� ������� �̷������.
		��� ���� �����ϰ� �ִ�.

		������, TCP �� �������� ��谡 �������� �ʴ´�.
		�ش� Ŭ���̾�Ʈ�� TCP Ŭ���̾�Ʈ�̴�.

		�� �̻��� write �Լ�ȣ��� ���޵� ���ڿ� ������ ������
		�ѹ��� ������ ���۵� �� �ִ�.

		����, �̷��� �Ǹ�, Ŭ���̾�Ʈ�� �ѹ��� �� �̻���
		���ڿ� ������ �����κ��� �ǵ��� �޾Ƽ�, ���ϴ� �����
		���� ���� ���� �ִ�.

		��, ���� ���忡����
		"���ڿ��� ���̰� ���� �� ���̴�, ���ڿ��� �� ���� ��Ŷ�� ������
		�����߰ڱ�" �̶�� �����ϰ�

		������ �ѹ��� write �Լ��� ȣ���, ������ ������ ���������
		������ �������� ũ�Ⱑ ũ�ٸ�,
		�ü���� ���������� �̸� ���� ���� �������� ������
		Ŭ���̾�Ʈ���� ������ ���� �ִ� ���̴�.

		�׸���, Ŭ���̾�Ʈ�� ��� ������ �����κ��� ���۵��� �ʾ������� �ұ��ϰ�
		read �Լ��� ȣ�������� �𸥴�.
		-----------------------------------------------------------------------------
		���� Client ���忡���� �ڽ��� ������ �޼��� ũ�⸸ŭ��
		���ڿ� �����Ͱ� �ٽ� ���ƿ� ������ ��ٸ��� ������ ������ ���̴�.
		*/
            strLen =
                send(hClntSock, m_InputText.c_str(), strlen(m_InputText.c_str()), 0);

            memset(messageBuffer, 0, sizeof(messageBuffer));

            while (readLen < strLen)
            {
                readCnt =
                    recv(hClntSock, &messageBuffer[readLen], BUF_SIZE - 1, 0);

                if (readCnt == -1)
                {
                    // NetworkUtil::ErrorHandling("recv() Error");
                    continue;
                }

                readLen += readCnt;
            }
            
            messageBuffer[readLen] = 0;

		    printf("Message from server : %s \n", messageBuffer);
        }
    }

    ImGui::End();
}

void EchoTCPClientLayer::connectServer()
{
    SOCKADDR_IN servAddr;

    char message[30];
    int strLen = 0, idx = 0, readLen = 0, readCnt = 0;

    // ���� ���̺귯�� �ʱ�ȭ
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        NetworkUtil::ErrorHandling("WSAStartUp() Error");

    // TCP ����
    hClntSock = socket(PF_INET, SOCK_STREAM, 0);

    // ���� ����
    if (hClntSock == INVALID_SOCKET)
        NetworkUtil::ErrorHandling("socket() Error");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(TEST_SERVER_IP_ADDRESS);
    servAddr.sin_port = htons(atoi(TEST_SERVER_PORT));

    // ������ ������ �������� ������ ���� ��û
    if (connect(hClntSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) ==
        SOCKET_ERROR)
    {
        NetworkUtil::ErrorHandling("connect() Error");
    }
    else
    {
        HZ_CORE_INFO("Connected to server");
    }

    // while (1)
    // {
    //     // ����ڷκ��� �Է��� �ޱ�
    //     fputs("Input message(Q to Quit) : ", stdout);
    //     fgets(message, BUF_SIZE, stdin);
    // 
    //     if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
    //         break;
    // 
    //     /*
	// 	�ش� �ڵ��, read, write �Լ��� ȣ��� ������
	// 	���ڿ� ������, ���� ������� �̷������.
	// 	��� ���� �����ϰ� �ִ�.
    // 
	// 	������, TCP �� �������� ��谡 �������� �ʴ´�.
	// 	�ش� Ŭ���̾�Ʈ�� TCP Ŭ���̾�Ʈ�̴�.
    // 
	// 	�� �̻��� write �Լ�ȣ��� ���޵� ���ڿ� ������ ������
	// 	�ѹ��� ������ ���۵� �� �ִ�.
    // 
	// 	����, �̷��� �Ǹ�, Ŭ���̾�Ʈ�� �ѹ��� �� �̻���
	// 	���ڿ� ������ �����κ��� �ǵ��� �޾Ƽ�, ���ϴ� �����
	// 	���� ���� ���� �ִ�.
    // 
	// 	��, ���� ���忡����
	// 	"���ڿ��� ���̰� ���� �� ���̴�, ���ڿ��� �� ���� ��Ŷ�� ������
	// 	�����߰ڱ�" �̶�� �����ϰ�
    // 
	// 	������ �ѹ��� write �Լ��� ȣ���, ������ ������ ���������
	// 	������ �������� ũ�Ⱑ ũ�ٸ�,
	// 	�ü���� ���������� �̸� ���� ���� �������� ������
	// 	Ŭ���̾�Ʈ���� ������ ���� �ִ� ���̴�.
    // 
	// 	�׸���, Ŭ���̾�Ʈ�� ��� ������ �����κ��� ���۵��� �ʾ������� �ұ��ϰ�
	// 	read �Լ��� ȣ�������� �𸥴�.
	// 	-----------------------------------------------------------------------------
	// 	���� Client ���忡���� �ڽ��� ������ �޼��� ũ�⸸ŭ��
	// 	���ڿ� �����Ͱ� �ٽ� ���ƿ� ������ ��ٸ��� ������ ������ ���̴�.
	// 	*/
    //     strLen = send(hClntSock, message, strlen(message), 0);
    // 
    //     readLen = 0;
    // 
    //     while (readLen < strLen)
    //     {
    //         hClntSock = recv(hClntSock, &message[readLen], BUF_SIZE - 1, 0);
    // 
    //         if (readCnt == -1)
    //             NetworkUtil::ErrorHandling("recv() Error");
    // 
    //         readLen += readCnt;
    //     }
    // 
    //     //message[strLen] = 0;
    //     message[readLen] = 0;
    // 
    // 
    //     printf("Message from server : %s \n", message);
    // }
}
