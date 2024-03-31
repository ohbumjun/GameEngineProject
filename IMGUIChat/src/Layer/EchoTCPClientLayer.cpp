#include "EchoTCPClientLayer.h"
#include "Util/Util.h"

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
    if (showConnectWindow)
    {
		ImGuiConnectWindow();
	}
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

    // Send button
    if (ImGui::Button("Send", ImVec2(100, 0)))
    {
        if (strlen(messageBuffer) > 0)
        {
            // Send message to server
            // send(sock, messageBuffer, strlen(messageBuffer), 0);
            // memset(
            //     messageBuffer,
            //     0,
            //     sizeof(
            //         messageBuffer)); // Clear message buffer after sending
        }
    }

    ImGui::End();
}

void EchoTCPClientLayer::ImGuiConnectWindow()
{
    ImGui::Begin("Connect", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Username:");
    ImGui::InputText("", username, 32);

    // Connect button
    if (ImGui::Button("Connect", ImVec2(100, 0)))
    {
        if (strlen(username) > 0)
        {
            // Initialize Winsock (Windows-specific)
            // WSADATA wsaData;
            // if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            // {
            //     printf("WSAStartup failed with error: %d\n", WSAGetLastError());
            //     return;
            // }
            //
            // // Create socket
            // sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            // if (sock == INVALID_SOCKET)
            // {
            //     printf("socket creation failed with error: %d\n",
            //            WSAGetLastError());
            //     return;
            // }
            //
            // // Setup server address
            // serverAddr.sin_family = AF_INET;
            // serverAddr.sin_port = htons(SERVER_PORT);
            // // inet_pton(AF_INET, SERVER_ADDRESS, &serverAddr.sin_addr);
            //
            // // Connect to server
            // if (connect(sock,
            //             (struct sockaddr *)&serverAddr,
            //             sizeof(serverAddr)) == SOCKET_ERROR)
            // {
            //     printf("connect failed with error: %d\n", WSAGetLastError());
            //     closesocket(sock);
            //     return;
            // }
            //
            // connected = true;
            // showConnectWindow = false;
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

    while (1)
    {
        // ����ڷκ��� �Է��� �ޱ�
        fputs("Input message(Q to Quit) : ", stdout);
        fgets(message, BUF_SIZE, stdin);

        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;

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
        strLen = send(hClntSock, message, strlen(message), 0);

        readLen = 0;

        while (readLen < strLen)
        {
            hClntSock = recv(hClntSock, &message[readLen], BUF_SIZE - 1, 0);

            if (readCnt == -1)
                NetworkUtil::ErrorHandling("recv() Error");

            readLen += readCnt;
        }

        //message[strLen] = 0;
        message[readLen] = 0;


        printf("Message from server : %s \n", message);
    }
}
