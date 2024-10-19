#pragma once
#include <Hazel.h>

class NetworkManager
{
	public:
	NetworkManager();
	~NetworkManager();

	void Init();
	void DisconnectFromServer();
	void SendData(const std::string& data);
	void ReceiveData();
	void Update();
	void Shutdown();

	private:
    static std::vector <PROCESS_INFORMATION> m_Pids;
};