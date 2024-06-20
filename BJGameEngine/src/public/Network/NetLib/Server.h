#pragma once
#include <memory>

#include "Network/Common/ErrorCode.h"


namespace NServerNetLib
{
	struct ServerConfig;
	class ILog;
	class ITcpNetwork;
}


namespace NLogicLib
{
	class UserManager;
	class LobbyManager;
	class PacketProcess;

	class Server
	{
	public:
		Server();
		~Server();

		NCommon::ERROR_CODE Init();

		void Run();

		void Stop();


	private:
		NCommon::ERROR_CODE LoadConfig();

		void Release();


	private:
		bool m_IsRun = false;

		std::unique_ptr<NServerNetLib::ServerConfig> m_pServerConfig;
		std::unique_ptr<NServerNetLib::ILog> m_pLogger;

		std::unique_ptr<NServerNetLib::ITcpNetwork> m_pNetwork;

		std::unique_ptr<PacketProcess> m_pPacketProc;

		// UserManager : 로그인된 유저 정보들을 저장한다.
		std::unique_ptr<UserManager> m_pUserMgr;

		// LobbyManager : 현재 서버 구조는 MMO 구조를 기반으로 하고자 한다.
		// Lobby. 에 대한 관리를 하는 Class 이다.
		// 그리고 각 Lobby 안에 Room 이 들어있는 구조가 된다.
		std::unique_ptr<LobbyManager> m_pLobbyMgr;

	};

}