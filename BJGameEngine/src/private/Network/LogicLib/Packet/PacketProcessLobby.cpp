#include <tuple>

#include "Network/Common/Packet.h"
#include "Network/Common/ErrorCode.h"
#include "Network/NetLib/TcpNetwork.h"
#include "Network/LogicLib/User/User.h"
#include "Network/LogicLib/User/UserManager.h"
#include "Network/LogicLib/Lobby/LobbyManager.h"
#include "Network/LogicLib/Lobby/Lobby.h"
#include "Network/LogicLib/Packet/PacketProcess.h"

using PACKET_ID = NCommon::PACKET_ID;

namespace NLogicLib
{
	ERROR_CODE PacketProcess::LobbyEnter(PacketInfo packetInfo)
	{
		auto reqPkt = (NCommon::PktLobbyEnterReq*)packetInfo.pRefData;
		NCommon::PktLobbyEnterRes resPkt;

		auto [errorCode, pUser] = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);

		if (errorCode != ERROR_CODE::NONE) {
			// 1) 유저가 없는 경우
			// 2) user 가 Confirm 이 아닌 경우
			resPkt.SetError(errorCode);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_ENTER_RES, sizeof(NCommon::PktLobbyEnterRes), (char*)&resPkt);
			return errorCode;
		}

		if (pUser->IsCurDomainInLogIn() == false) 
		{
			// User 가 Login 상태가 아니라면
			resPkt.SetError(ERROR_CODE::LOBBY_ENTER_INVALID_DOMAIN);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_ENTER_RES, sizeof(NCommon::PktLobbyEnterRes), (char*)&resPkt);
			return ERROR_CODE::LOBBY_ENTER_INVALID_DOMAIN;
		}

		auto pLobby = m_pRefLobbyMgr->GetLobby(reqPkt->LobbyId);

		if (pLobby == nullptr) 
		{
			// User 가 요청한 Lobby 가 존재하지 않는다면
			resPkt.SetError(ERROR_CODE::LOBBY_ENTER_INVALID_LOBBY_INDEX);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_ENTER_RES, sizeof(NCommon::PktLobbyEnterRes), (char*)&resPkt);
			return ERROR_CODE::LOBBY_ENTER_INVALID_LOBBY_INDEX;
		}

		ERROR_CODE enterRet = pLobby->EnterUser(pUser);

		if (enterRet != ERROR_CODE::NONE) {
			// 1) Lobby 최대 인원을 초과
			// 2) 같은 User 존재
			// 3) 들어갈 수 있는 User 가 없음... -> 1과 동일한 거 아닌가 ?
			resPkt.SetError(enterRet);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_ENTER_RES, sizeof(NCommon::PktLobbyEnterRes), (char*)&resPkt);
			return enterRet;
		}

		// User 를 Lobby 에 넣어주고
		// 관련 res pkt 을 Lobby 입장 요청한 user Client 에게 보내준다.
		resPkt.MaxUserCount = pLobby->MaxUserCount();
		resPkt.MaxRoomCount = pLobby->MaxRoomCount();
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_ENTER_RES, sizeof(NCommon::PktLobbyEnterRes), (char*)&resPkt);
		return ERROR_CODE::NONE;
	}

	ERROR_CODE PacketProcess::LobbyLeave(PacketInfo packetInfo)
	{
		NCommon::PktLobbyLeaveRes resPkt;

		auto [errorCode, pUser] = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);

		if (errorCode != ERROR_CODE::NONE) {
			resPkt.SetError(errorCode);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_LEAVE_RES, sizeof(NCommon::PktLobbyLeaveRes), (char*)&resPkt);
			return errorCode;
		}

		if (pUser->IsCurDomainInLobby() == false) {
			resPkt.SetError(ERROR_CODE::LOBBY_LEAVE_INVALID_DOMAIN);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_LEAVE_RES, sizeof(NCommon::PktLobbyLeaveRes), (char*)&resPkt);
			return ERROR_CODE::LOBBY_LEAVE_INVALID_DOMAIN;
		}

		Lobby* pLobby = m_pRefLobbyMgr->GetLobby(pUser->GetLobbyIndex());
		if (pLobby == nullptr) {
			resPkt.SetError(ERROR_CODE::LOBBY_LEAVE_INVALID_LOBBY_INDEX);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_LEAVE_RES, sizeof(NCommon::PktLobbyLeaveRes), (char*)&resPkt);
			return ERROR_CODE::LOBBY_LEAVE_INVALID_LOBBY_INDEX;
		}

		ERROR_CODE enterRet = pLobby->LeaveUser(pUser->GetIndex());

		if (enterRet != ERROR_CODE::NONE) 
		{
			resPkt.SetError(enterRet);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_LEAVE_RES, sizeof(NCommon::PktLobbyLeaveRes), (char*)&resPkt);
			return enterRet;
		}

		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_LEAVE_RES, sizeof(NCommon::PktLobbyLeaveRes), (char*)&resPkt);
		
		return ERROR_CODE::NONE;
	}
}