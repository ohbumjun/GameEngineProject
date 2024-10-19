#pragma once

#include <memory>
#include "../../Common/Packet.h"
#include "../../Common/ErrorCode.h"
#include "../../NetLib/Define.h"

namespace NServerNetLib
{
	class ITcpNetwork;
	class ILog;
}
	
class UserManager;
class LobbyManager;

namespace NLogicLib
{
	class ConnectedUserManager;
	class UserManager;
	class LobbyManager;

	using ServerConfig = NServerNetLib::ServerConfig;

	class PacketProcess
	{
		using PacketInfo = NServerNetLib::RecvPacketInfo;
		// retur type : ERROR_CODE
		// parameter : PacketInfo
		// PackeProcess Class 의 멤버함수 -> PacketFunc 이름의 함수 포인트
		typedef NCommon::ERROR_CODE(PacketProcess::* PacketFunc)(PacketInfo);

		// 함수 포인트들의 배열
		// - 모든 종류의 패킷을 처리하는 함수 포인터들을 배열 형태로 관리.
		PacketFunc PacketFuncArray[(int)NCommon::PACKET_ID::MAX];

		using TcpNet = NServerNetLib::ITcpNetwork;
		using ILog = NServerNetLib::ILog;
	public:
		PacketProcess();
		~PacketProcess();

		void Init(TcpNet* pNetwork, UserManager* pUserMgr, LobbyManager* pLobbyMgr, ServerConfig* pConfig, ILog* pLogger);

		void Process(PacketInfo packetInfo);

		void StateCheck();

	private:
		// 아래의 대상들 모두 TcpNetwork class 에 정의된 객체들을 가리키는 포인터
		ILog* m_pRefLogger;
		TcpNet* m_pRefNetwork;
		UserManager* m_pRefUserMgr;
		LobbyManager* m_pRefLobbyMgr;

		std::unique_ptr<ConnectedUserManager> m_pConnectedUserManager;
	private:
		NCommon::ERROR_CODE NtfSysConnctSession(PacketInfo packetInfo);
		NCommon::ERROR_CODE NtfSysCloseSession(PacketInfo packetInfo);

		NCommon::ERROR_CODE Login(PacketInfo packetInfo);

		NCommon::ERROR_CODE LobbyList(PacketInfo packetInfo);

		NCommon::ERROR_CODE LobbyEnter(PacketInfo packetInfo);

		NCommon::ERROR_CODE LobbyLeave(PacketInfo packetInfo);

		NCommon::ERROR_CODE RoomEnter(PacketInfo packetInfo);

		NCommon::ERROR_CODE RoomLeave(PacketInfo packetInfo);

		NCommon::ERROR_CODE RoomChat(PacketInfo packetInfo);

		NCommon::ERROR_CODE RoomMasterGameStart(PacketInfo packetInfo);

		NCommon::ERROR_CODE RoomGameStart(PacketInfo packetInfo);

		NCommon::ERROR_CODE DevEcho(PacketInfo packetInfo);

	};
}