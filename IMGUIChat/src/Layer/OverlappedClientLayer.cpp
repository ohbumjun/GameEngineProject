#include "OverlappedClientLayer.h"
#include "Hazel/Core/Application/Application.h"
#include "Hazel/Core/ID/UUID.h"
#include "ServerInfo.h"
#include "Util/Util.h"

OverlappedClientLayer::~OverlappedClientLayer()
{
}

void OverlappedClientLayer::OnAttach()
{
    initialize();
}

void OverlappedClientLayer::OnDetach()
{
    WSACloseEvent(evObj);
    closesocket(hSocket);
    WSACleanup();
}

void OverlappedClientLayer::OnUpdate(Hazel::Timestep ts)
{
}

void OverlappedClientLayer::OnEvent(Hazel::Event &event)
{
}

void OverlappedClientLayer::OnImGuiRender()
{
    ImGuiChatWindow();
}

void OverlappedClientLayer::ImGuiChatWindow()
{
    ImGui::Begin("Chat", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    char messageBuffer[256];
    memset(messageBuffer, 0, sizeof(messageBuffer));
    strcpy_s(messageBuffer, sizeof(messageBuffer), inputText.c_str());

    if (ImGui::InputText("##Name", messageBuffer, sizeof(messageBuffer)))
    {
        inputText = messageBuffer;
    }

    // Send button
    if (ImGui::Button("Send", ImVec2(100, 0)))
    {
        if (inputText.length() > 0)
        {
            int sendBytes = 0;
            char msg[BUF_SIZE];

            memcpy(msg, inputText.c_str(), inputText.length() + 1);
            
            dataBuf.len = strlen(msg) - 1;
            dataBuf.buf = msg;

            // WSASend �Լ��� ȣ���ϰ����� ������ IO�� �������̶��, ��, �����͸� ����ؼ� �������̶��
            // ���� ���ϰ��� SOCKET_ERROR �� �ƴ϶��,
            // �ش� �Լ� ȣ��� ���ÿ� ������ ������ �Ϸ�� ��, �׸��� sendBytes ���� ������ �������� ũ�Ⱑ ����ִ�.
            // IO �� �Ϸ�Ǹ� WSAOverlapped ����ü ������ �����ϴ� Event ������Ʈ�� signaled ���°� �ȴ�.
            if (WSASend(hSocket,
                        &dataBuf,
                        1,
                        (LPDWORD)&sendBytes,
                        0,
                        &overlapped,
                        NULL) == SOCKET_ERROR)
            {
                // WSASend �Լ��� ȣ���ϰ����� ������ IO�� �������̶��, ��, �����͸� ����ؼ� �������̶��
                if (WSAGetLastError() == WSA_IO_PENDING)
                {
                    puts("Background data send");

                    // �ش� IO�� ������ overlapped.hEvent, �� evObj �̺�Ʈ Ŀ�� ������Ʈ�� signaled ���°� �ȴ�
                    // signaled ���°� �� ������ ��ٸ���. (WSA_INFINITE)
                    WSAWaitForMultipleEvents(1,
                                             &evObj,
                                             TRUE,
                                             WSA_INFINITE,
                                             FALSE);

                    // ���� ���۵� �������� ũ�⸦ Ȯ��
                    // 4��° : fwait -> ������ io �� �������� ��Ȳ�� ���, true ���޽� io �� �Ϸ�� ������
                    //							����ϰ� �ǰ� false �� ���޽� FALSE ��ȯ�ϸ鼭 �Լ� �������´�.
                    //	5��° : oob �޽��� �� �ΰ� ������ Ȯ���ϱ� ���� ����
                    WSAGetOverlappedResult(hSocket,
                                           &overlapped,
                                           (LPDWORD)&sendBytes,
                                           FALSE,
                                           NULL);
                }
                else
                {
                    NetworkUtil::ErrorHandling("WSASend() Error");
                }
            }
        }
    }

    ImGui::End();
}

void OverlappedClientLayer::initialize()
{
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        NetworkUtil::ErrorHandling("WSAStartUp() Error");

    // WSA_FLAG_OVERLAPPED : overlapped io �� ������ ������ ����
    hSocket = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

    memset(&sendAddr, 0, sizeof(sendAddr));
    sendAddr.sin_family = AF_INET;
    sendAddr.sin_addr.s_addr = inet_addr(TEST_SERVER_IP_ADDRESS);
    sendAddr.sin_port = htons(atoi(TEST_SERVER_PORT));

    if (connect(hSocket, (SOCKADDR *)&sendAddr, sizeof(sendAddr)) ==
        SOCKET_ERROR)
        NetworkUtil::ErrorHandling("connect() Error");

    // manual-reset, non-signaled ������ �̺�Ʈ ����
    evObj = WSACreateEvent();

    // ��� ��Ʈ 0���� �ʱ�ȭ
    memset(&overlapped, 0, sizeof(overlapped));

    // �ش� overlapped ����ü ������ event ������Ʈ�� ����Ͽ� �������� ���� ����
    // �Ϸ�Ǿ����� ���� Ȯ���Ѵ�.
    overlapped.hEvent = evObj;
}

void OverlappedClientLayer::receiveMessage(Hazel::UUID threadId)
{
}

void OverlappedClientLayer::receiveConnection()
{
}
