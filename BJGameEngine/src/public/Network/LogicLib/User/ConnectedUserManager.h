#pragma once

#include <time.h>
#include <chrono>
#include <vector>
#include "../../NetLib/ILog.h"
#include "../../NetLib/TcpNetwork.h"

namespace NLogicLib
{
	// 일반 user Class 와 Connected user 를 분리하는 이유가 무엇인가 ?
	struct ConnectedUser
	{
		void Clear()
		{
			m_IsLoginSuccess = false;
			m_ConnectedTime = 0;
		}

		bool m_IsLoginSuccess = false;

		// 해당 User 혹은 Client 가 connect 된 시간 ?
		// ex) ConnectedUserManager::SetConnectSession
		time_t m_ConnectedTime = 0;
	};

	// 굳이 UserManager 가 있는데도 이 Class 를 만든 이유가 있나 ?
	class ConnectedUserManager
	{
		using TcpNet = NServerNetLib::ITcpNetwork;
		using ILog = NServerNetLib::ILog;
	public:
		ConnectedUserManager() {};
		virtual ~ConnectedUserManager() {};

		// ex) PacketProcess::Init 
		void Init(const int maxSessionCount, TcpNet* pNetwork, NServerNetLib::ServerConfig* pConfig, ILog* pLogger);

		void SetConnectSession(const int sessionIndex);
		void SetLogin(const int sessionIndex);
		void SetDisConnectSession(const int sessionIndex);
		void LoginCheck();

	private:
		ILog* m_pRefLogger;
		TcpNet* m_pRefNetwork;

		// 최대 client Session 개수 만큼 일단 Init 함수에서 만들어둔다.
		std::vector<ConnectedUser> ConnectedUserList;

		bool m_IsLoginCheck = false;

		std::chrono::system_clock::time_point m_LatestLoginCheckTime = std::chrono::system_clock::now();
		int m_LatestLogincheckIndex = -1;
	};
}
