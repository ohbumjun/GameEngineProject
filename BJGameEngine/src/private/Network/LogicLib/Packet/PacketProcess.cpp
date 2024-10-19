#include <cstring>

#include "Network/NetLib/ILog.h"
#include "Network/NetLib/TcpNetwork.h"
#include "Network/LogicLib/User/User.h"
#include "Network/LogicLib/User/UserManager.h"
#include "Network/LogicLib/Lobby/LobbyManager.h"
#include "Network/LogicLib/Lobby/Lobby.h"
#include "Network/LogicLib/Room/Room.h"
#include "Network/LogicLib/User/ConnectedUserManager.h"
//#include "Room.h"
#include "Network/LogicLib/Room/RoomManager.h"
#include "Network/LogicLib/Packet/PacketProcess.h"

using LOG_TYPE = NServerNetLib::LOG_TYPE;
using ServerConfig = NServerNetLib::ServerConfig;

namespace NLogicLib
{
	PacketProcess::PacketProcess() {}
	PacketProcess::~PacketProcess() {}

	void PacketProcess::Init(TcpNet* pNetwork, UserManager* pUserMgr, LobbyManager* pLobbyMgr, ServerConfig* pConfig, ILog* pLogger)
	{
		m_pRefLogger = pLogger;
		m_pRefNetwork = pNetwork;
		m_pRefUserMgr = pUserMgr;
		m_pRefLobbyMgr = pLobbyMgr;

		m_pConnectedUserManager = std::make_unique<ConnectedUserManager>();
		m_pConnectedUserManager->Init(pNetwork->ClientSessionPoolSize(), pNetwork, pConfig, pLogger);

		using netLibPacketId = NServerNetLib::PACKET_ID;
		using commonPacketId = NCommon::PACKET_ID;
		for (int i = 0; i < (int)commonPacketId::MAX; ++i)
		{
			PacketFuncArray[i] = nullptr;
		}

		PacketFuncArray[(int)netLibPacketId::NTF_SYS_CONNECT_SESSION] = &PacketProcess::NtfSysConnctSession;
		PacketFuncArray[(int)netLibPacketId::NTF_SYS_CLOSE_SESSION] = &PacketProcess::NtfSysCloseSession;
		PacketFuncArray[(int)commonPacketId::LOGIN_IN_REQ] = &PacketProcess::Login;
		PacketFuncArray[(int)commonPacketId::LOBBY_LIST_REQ] = &PacketProcess::LobbyList;
		PacketFuncArray[(int)commonPacketId::LOBBY_ENTER_REQ] = &PacketProcess::LobbyEnter;
		PacketFuncArray[(int)commonPacketId::LOBBY_LEAVE_REQ] = &PacketProcess::LobbyLeave;
		PacketFuncArray[(int)commonPacketId::ROOM_ENTER_REQ] = &PacketProcess::RoomEnter;
		PacketFuncArray[(int)commonPacketId::ROOM_LEAVE_REQ] = &PacketProcess::RoomLeave;
		PacketFuncArray[(int)commonPacketId::ROOM_CHAT_REQ] = &PacketProcess::RoomChat;
		PacketFuncArray[(int)commonPacketId::ROOM_MASTER_GAME_START_REQ] = &PacketProcess::RoomMasterGameStart;
		PacketFuncArray[(int)commonPacketId::ROOM_GAME_START_REQ] = &PacketProcess::RoomGameStart;



		PacketFuncArray[(int)commonPacketId::DEV_ECHO_REQ] = &PacketProcess::DevEcho;
	}

	void PacketProcess::Process(PacketInfo packetInfo)
	{
		auto packetId = packetInfo.PacketId;

		if (PacketFuncArray[packetId] == nullptr)
		{
			return;
		}

		(this->*PacketFuncArray[packetId])(packetInfo);
	}

	NCommon::ERROR_CODE PacketProcess::NtfSysConnctSession(PacketInfo packetInfo)
	{
		/*
		- 클라이언트가 자기가 연결되었다고 패킷을 보내지는 않는다.
		  클라이언트 측에서 connect 함수를 호출할 뿐이다.
		- TcpNetwork::NewSession() 측에서 새로운 Session 을 만들어낼 때
		  만들어내는 Packet 이다.
		  음..서버 측에서 accept 를 한 이후에, 새롭게 클라이언트 연결 소켓을 만들어낼 때
		  만들어주는 Packet 정보로 보인다.
		*/

		m_pConnectedUserManager->SetConnectSession(packetInfo.SessionIndex);
		m_pRefLogger->Write(LOG_TYPE::L_INFO, "%s | NtfSysConnctSession. sessionIndex(%d)", __FUNCTION__, packetInfo.SessionIndex);
		return NCommon::ERROR_CODE::NONE;
	}

	NCommon::ERROR_CODE PacketProcess::NtfSysCloseSession(PacketInfo packetInfo)
	{
		User* pUser = std::get<1>(m_pRefUserMgr->GetUser(packetInfo.SessionIndex));

		if (pUser)
		{
			Lobby* pLobby = m_pRefLobbyMgr->GetLobby(pUser->GetLobbyIndex());
			if (pLobby)
			{
				Room* pRoom = pLobby->GetRoom(pUser->GetRoomIndex());

				if (pRoom)
				{
					// 만약 User 가 들어있는 Room 이 있다면 User 를 Room 에서 내보낸다.
					pRoom->LeaveUser(pUser->GetIndex());

					// Room 에서 나간 User 정보를 다른 User 들에게 알린다.
					pRoom->NotifyLeaveUserInfo(pUser->GetID().c_str());

					m_pRefLogger->Write(LOG_TYPE::L_INFO, "%s | NtfSysCloseSesson. sessionIndex(%d). Room Out", __FUNCTION__, packetInfo.SessionIndex);
				}

				// Lobby 에서도 User 를 내보낸다.
				pLobby->LeaveUser(pUser->GetIndex());

				m_pRefLogger->Write(LOG_TYPE::L_INFO, "%s | NtfSysCloseSesson. sessionIndex(%d). Lobby Out", __FUNCTION__, packetInfo.SessionIndex);
			}

			// User 정보를 삭제한다.
			m_pRefUserMgr->RemoveUser(packetInfo.SessionIndex);
		}

		m_pConnectedUserManager->SetDisConnectSession(packetInfo.SessionIndex);

		m_pRefLogger->Write(LOG_TYPE::L_INFO, "%s | NtfSysCloseSesson. sessionIndex(%d)", __FUNCTION__, packetInfo.SessionIndex);
		return ERROR_CODE::NONE;
	}

	ERROR_CODE PacketProcess::DevEcho(PacketInfo packetInfo)
	{
		auto reqPkt = (NCommon::PktDevEchoReq*)packetInfo.pRefData;

		NCommon::PktDevEchoRes resPkt;
		resPkt.ErrorCode = (short)ERROR_CODE::NONE;
		resPkt.DataSize = reqPkt->DataSize;
		std::memcpy(&resPkt.Datas, reqPkt->Datas, reqPkt->DataSize);

		auto sendSize = sizeof(NCommon::PktDevEchoRes) - (NCommon::DEV_ECHO_DATA_MAX_SIZE - reqPkt->DataSize);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::DEV_ECHO_RES, (short)sendSize, (char*)&resPkt);

		return ERROR_CODE::NONE;
	}


}