#include <thread>
#include <chrono>


#include "Network/NetLib/ServerNetErrorCode.h"
#include "Network/NetLib/Define.h"
#include "Network/NetLib/TcpNetwork.h"
#include "Network/Common/ConsoleLogger.h"
#include "Network/LogicLib/Room/RoomManager.h"
#include "Network/LogicLib/Packet/PacketProcess.h"
#include "Network/LogicLib/User/UserManager.h"
#include "Network/LogicLib/Lobby/LobbyManager.h"
#include "Network/NetLib/Server.h"

using NET_ERROR_CODE = NServerNetLib::NET_ERROR_CODE;
using LOG_TYPE = NServerNetLib::LOG_TYPE;

namespace NLogicLib
{
	Server::Server()
	{
	}

	Server::~Server()
	{
		Release();
	}

	NCommon::ERROR_CODE Server::Init()
	{
		m_pLogger = std::make_unique<ConsoleLog>();

		LoadConfig();

		m_pNetwork = std::make_unique<NServerNetLib::TcpNetwork>();
		auto result = m_pNetwork->Init(m_pServerConfig.get(), m_pLogger.get());

		if (result != NET_ERROR_CODE::NONE)
		{
			m_pLogger->Write(LOG_TYPE::L_ERROR, "%s | Init Fail. NetErrorCode(%s)", __FUNCTION__, (short)result);
			return NCommon::ERROR_CODE::MAIN_INIT_NETWORK_INIT_FAIL;
		}


		m_pUserMgr = std::make_unique<UserManager>();
		m_pUserMgr->Init(m_pServerConfig->MaxClientCount);

		m_pLobbyMgr = std::make_unique<LobbyManager>();
		m_pLobbyMgr->Init({ m_pServerConfig->MaxLobbyCount,
							m_pServerConfig->MaxLobbyUserCount,
							m_pServerConfig->MaxRoomCountByLobby,
							m_pServerConfig->MaxRoomUserCount },
			m_pNetwork.get(), m_pLogger.get());


		m_pPacketProc = std::make_unique<PacketProcess>();
		m_pPacketProc->Init(m_pNetwork.get(), m_pUserMgr.get(),
			m_pLobbyMgr.get(), m_pServerConfig.get(), m_pLogger.get());

		m_IsRun = true;

		m_pLogger->Write(LOG_TYPE::L_INFO, "%s | Init Success. Server Run", __FUNCTION__);
		return NCommon::ERROR_CODE::NONE;
	}

	void Server::Release()
	{
		if (m_pNetwork) {
			m_pNetwork->Release();
		}
	}

	void Server::Stop()
	{
		m_IsRun = false;
	}

	void Server::Run()
	{
		while (m_IsRun)
		{
			// Run 함수 내부에서 수신된 패킷 정보를 PacketQueue 에 넣어준다.
			m_pNetwork->Run();

			// while 문을 돌면서 발생한 이벤트들을 처리해준다.
			while (true)
			{
				// PacketQueue 에 있는 Packet 맨 앞에 있는 정보를 가져온다.
				NServerNetLib::RecvPacketInfo packetInfo = m_pNetwork->GetPacketInfo();

				// 0 이라는 것은, 유의미한 이벤트가 없다는 의미이다.
				// ex) 수신한 녀석도 없고, 전송한 녀석도 없고
				if (packetInfo.PacketId == 0)
				{
					break;
				}
				else
				{
					// Packet 의 PacketId 변수에 
					// 어떤 type 의 패킷인지가 저장되어 있다.
					// 해당 type 을 기준으로 패킷 처리를 한다.
					m_pPacketProc->Process(packetInfo);
				}
			}
		}
	}

	NCommon::ERROR_CODE Server::LoadConfig()
	{
		m_pServerConfig = std::make_unique<NServerNetLib::ServerConfig>();

		m_pServerConfig->Port = 32452;
		m_pServerConfig->BackLogCount = 128;

		// 최대 클라이언트를 몇개까지 받을 것인가.
		m_pServerConfig->MaxClientCount = 1000;

		m_pServerConfig->MaxClientSockOptRecvBufferSize = 10240;
		m_pServerConfig->MaxClientSockOptSendBufferSize = 10240;
		m_pServerConfig->MaxClientRecvBufferSize = 8192;
		m_pServerConfig->MaxClientSendBufferSize = 8192;

		// MaxClientCount 에서 여유를 더 주는 것이다.
		// 여유를 더 주는 이유는 여기 client 개수가 많아지면 서버단에서 잘라버린다.
		// 그런데 클라이언트에서는 그 이유를 알 수 없다..
		// 그래서 ... 존재하는 변수..?
		m_pServerConfig->ExtraClientCount = 64;
		m_pServerConfig->MaxLobbyCount = 2;
		m_pServerConfig->MaxRoomCountByLobby = 20;
		m_pServerConfig->MaxRoomUserCount = 4;


		m_pLogger->Write(NServerNetLib::LOG_TYPE::L_INFO, "%s | Port(%d), Backlog(%d)", __FUNCTION__, m_pServerConfig->Port, m_pServerConfig->BackLogCount);
		m_pLogger->Write(NServerNetLib::LOG_TYPE::L_INFO, "%s | IsLoginCheck(%d)", __FUNCTION__, m_pServerConfig->IsLoginCheck);
		return NCommon::ERROR_CODE::NONE;
	}

}