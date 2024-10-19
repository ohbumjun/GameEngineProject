#include <tuple>

#include "Network/Common/Packet.h"
#include "Network/Common/ErrorCode.h"
#include "Network/LogicLib/User/User.h"
#include "Network/NetLib/TcpNetwork.h"
#include "Network/LogicLib/User/UserManager.h"
#include "Network/LogicLib/Lobby/LobbyManager.h"
#include "Network/LogicLib/Lobby/Lobby.h"
#include "Network/LogicLib/Game/Game.h"
#include "Network/LogicLib/Room/Room.h"
#include "Network/LogicLib/Packet/PacketProcess.h"

using PACKET_ID = NCommon::PACKET_ID;

namespace NLogicLib
{
	NCommon::ERROR_CODE PacketProcess::RoomEnter(PacketInfo packetInfo)
	{
		// 클라이언트로부터 온 Packet 은 RoomEnterReq 이다.
		NCommon::PktRoomEnterReq* reqPkt = (NCommon::PktRoomEnterReq*)packetInfo.pRefData;

		// 그에 대한 response packet 변수
		NCommon::PktRoomEnterRes resPkt;

		// 해당 Client Session 에 대응되는 User 정보를 얻어온다.
		const std::tuple<NCommon::ERROR_CODE, User*> userTuple = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);

		NCommon::ERROR_CODE errorCode = std::get<0>(userTuple);
		User* pUser = std::get<1>(userTuple);

		if (errorCode != NCommon::ERROR_CODE::NONE)
		{
			// 1) 해당 session Index 에 대응되는 User 가 없거나
			// 2) User 가 Confirm 이지 않은 경우
			resPkt.SetError(errorCode);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_ENTER_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		// 해당 Packet 에 대응되는 user 를 찾았는데 Lobby 에 아직 들어있지 않은 경우
		if (pUser->IsCurDomainInLobby() == false)
		{
			// User 가 현재 Lobby 에 있지 않은 경우
			resPkt.SetError(ERROR_CODE::ROOM_ENTER_INVALID_DOMAIN);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_ENTER_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_ENTER_INVALID_LOBBY_INDEX;
		}

		// 자. 이제 User 가 들어갈 Room 을 배정해주면 된다.
		Room* pRoom = nullptr;

		if (reqPkt->IsCreate)
		{
			// 새로운 Room 을 만들어야 하는 경우
		}
		else
		{
			// 기존 Room 에 들어가야 하는 경우
		}
	}

	ERROR_CODE PacketProcess::RoomLeave(PacketInfo packetInfo)
	{
		NCommon::PktRoomLeaveRes resPkt;

		// 현재 나가고자 하는 Client Session 의 User 정보를 가져온다.
		auto [errorCode, pUser] = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);

		if (errorCode != ERROR_CODE::NONE) 
		{
			// 1)  해당 Session 에 대응되는 User 가 없거나
			// 2) User 가 Confirm 되지 않은 경우
			resPkt.SetError(errorCode);

			m_pRefNetwork->SendData(packetInfo.SessionIndex, 
				(short)PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);

			return errorCode;
		}

		// UserManager 의 m_UserObjPoolIndex
		auto userIndex = pUser->GetIndex();

		if (pUser->IsCurDomainInRoom() == false) 
		{
			// User 가 현재 Room 에 존재하지 않는 경우
			resPkt.SetError(ERROR_CODE::ROOM_LEAVE_INVALID_DOMAIN);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_LEAVE_INVALID_DOMAIN;
		}

		// User 가 속한 Lobby Idx 정보를 가져온다.
		// -> Room 에서 나간 이후, Lobby 로 되돌아가게 하기 위함으로 보인다.
		auto lobbyIndex = pUser->GetLobbyIndex();

		Lobby* pLobby = m_pRefLobbyMgr->GetLobby(lobbyIndex);

		if (pLobby == nullptr) 
		{
			// User 가 속한 Lobby 가 존재하지 않는 경우
			resPkt.SetError(ERROR_CODE::ROOM_ENTER_INVALID_LOBBY_INDEX);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_ENTER_INVALID_LOBBY_INDEX;
		}

		// User 가 속한 Room 정보를 가져온다.
		Room* pRoom = pLobby->GetRoom(pUser->GetRoomIndex());

		if (pRoom == nullptr) 
		{
			resPkt.SetError(ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX;
		}

		// 해당 Room 에서 User 를 빼준다.
		auto leaveRet = pRoom->LeaveUser(userIndex);
		if (leaveRet != ERROR_CODE::NONE) 
		{
			// 1) 해당 Room 에 User 가 존재하지 않는 경우
			// 2) 해당 Room 이 사용중이지 않은 상태였다면
			resPkt.SetError(leaveRet);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);
			return leaveRet;
		}

		// 유저 정보를 로비로 변경
		pUser->EnterLobby(lobbyIndex);

		// 룸에 유저가 나갔음을 통보
		pRoom->NotifyLeaveUserInfo(pUser->GetID().c_str());

		// User Client 측에 RoomLeaveRes 패킷을 보낸다.
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);
		return ERROR_CODE::NONE;
	}

	ERROR_CODE PacketProcess::RoomChat(PacketInfo packetInfo)
	{
		NCommon::PktRoomChatReq* reqPkt = (NCommon::PktRoomChatReq*)packetInfo.pRefData;
		NCommon::PktRoomChatRes resPkt;

		auto [errorCode, pUser] = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);

		if (errorCode != ERROR_CODE::NONE) 
		{
			// 1) 해당 Session 에 대응되는 User 가 없거나
			// 2) User 가 Confirm 되지 않은 경우 (로그인 되지 않은 상태인가.?)
			resPkt.SetError(errorCode);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		// 현재 Room 에 속해있는 User 가 아닌 경우
		if (pUser->IsCurDomainInRoom() == false) {
			resPkt.SetError(ERROR_CODE::ROOM_CHAT_INVALID_DOMAIN);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_CHAT_INVALID_DOMAIN;
		}

		auto lobbyIndex = pUser->GetLobbyIndex();
		auto pLobby = m_pRefLobbyMgr->GetLobby(lobbyIndex);
		if (pLobby == nullptr) {
			// 현재 해당 User 가 속한 Lobby 가 없는 경우
			resPkt.SetError(ERROR_CODE::ROOM_CHAT_INVALID_LOBBY_INDEX);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_CHAT_INVALID_LOBBY_INDEX;
		}

		auto pRoom = pLobby->GetRoom(pUser->GetRoomIndex());

		if (pRoom == nullptr) {
			// 현재 해당 User 가 속한 Room 이 없는 경우
			resPkt.SetError(ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX;
		}

		// 현재 User 를 제외한 나머지 User 에게 채팅 메시지를 전달한다.
		pRoom->NotifyChat(pUser->GetSessioIndex(), pUser->GetID().c_str(), reqPkt->Msg);

		// 현재 User 에 대응되는 Client 에게 채팅 Packet 을 보낸다.
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
		
		return ERROR_CODE::NONE;
	}

	ERROR_CODE PacketProcess::RoomMasterGameStart(PacketInfo packetInfo)
	{
		NCommon::PktRoomMaterGameStartRes resPkt;

		auto [errorCode, pUser] = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);

		if (errorCode != ERROR_CODE::NONE) {
			// 1) 해당 Session 에 대응되는 User 가 없거나
			// 2) User 가 Confirm 되지 않은 경우
			resPkt.SetError(errorCode);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_MASTER_GAME_START_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		if (pUser->IsCurDomainInRoom() == false) {
			// User 가 현재 Room 에 존재하지 않는 경우
			resPkt.SetError(ERROR_CODE::ROOM_MASTER_GAME_START_INVALID_DOMAIN);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_MASTER_GAME_START_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_MASTER_GAME_START_INVALID_DOMAIN;
		}

		auto lobbyIndex = pUser->GetLobbyIndex();
		auto pLobby = m_pRefLobbyMgr->GetLobby(lobbyIndex);
		if (pLobby == nullptr) {
			// User 가 속한 Lobby 가 존재하지 않는 경우
			resPkt.SetError(ERROR_CODE::ROOM_MASTER_GAME_START_INVALID_LOBBY_INDEX);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_MASTER_GAME_START_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_MASTER_GAME_START_INVALID_LOBBY_INDEX;
		}

		auto pRoom = pLobby->GetRoom(pUser->GetRoomIndex());
		if (pRoom == nullptr) {
			// User 가 속한 Room 이 존재하지 않는 경우
			resPkt.SetError(ERROR_CODE::ROOM_MASTER_GAME_START_INVALID_ROOM_INDEX);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_MASTER_GAME_START_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_MASTER_GAME_START_INVALID_ROOM_INDEX;
		}

		// 방장이 맞는지 확인
		if (pRoom->IsMaster(pUser->GetIndex()) == false) {
			resPkt.SetError(ERROR_CODE::ROOM_MASTER_GAME_START_INVALID_MASTER);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_MASTER_GAME_START_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_MASTER_GAME_START_INVALID_MASTER;
		}

		// 방의 인원이 2명인가?
		// 참고 : 이 조건을 변경해야 할 수도 있다. 2명 이상이 아닌 경우라면 시작 X, 2명 이상이면 시작 가능
		if (pRoom->GetUserCount() != 2) {
			resPkt.SetError(ERROR_CODE::ROOM_MASTER_GAME_START_INVALID_USER_COUNT);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_MASTER_GAME_START_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_MASTER_GAME_START_INVALID_USER_COUNT;
		}

		// 방의 상태가 게임을 안하는 중인지?
		if (pRoom->GetGameObj()->GetState() != GameState::NONE) {
			// STARTTING, ING, END 중 하나인 경우
			resPkt.SetError(ERROR_CODE::ROOM_MASTER_GAME_START_INVALID_GAME_STATE);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_MASTER_GAME_START_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_MASTER_GAME_START_INVALID_GAME_STATE;
		}


		// 방의 게임 상태 변경
		pRoom->GetGameObj()->SetState(GameState::STARTTING);

		// 방의 다른 유저에게 방장이 게임 시작 요청을 했음을 알리고
		pRoom->SendToAllUser((short)PACKET_ID::ROOM_MASTER_GAME_START_NTF,
			0,			// data size 는 0
			nullptr,	// 해당 Packet 에 별다른 Data 는 없다. 그저 Packet ID 만으로 판별
			pUser->GetIndex());

		// Master Game Start 요청 Client에게 답변을 보낸다.
		m_pRefNetwork->SendData(
			packetInfo.SessionIndex, 
			(short)PACKET_ID::ROOM_MASTER_GAME_START_RES, 
			sizeof(resPkt), 
			(char*)&resPkt
		);
		
		return ERROR_CODE::NONE;
	}

	ERROR_CODE PacketProcess::RoomGameStart(PacketInfo packetInfo)
	{
		NCommon::PktRoomGameStartRes resPkt;

		auto [errorCode, pUser] = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);

		if (errorCode != ERROR_CODE::NONE) {
			resPkt.SetError(errorCode);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_GAME_START_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		if (pUser->IsCurDomainInRoom() == false) {
			resPkt.SetError(ERROR_CODE::ROOM_MASTER_GAME_START_INVALID_DOMAIN);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_GAME_START_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_MASTER_GAME_START_INVALID_DOMAIN;
		}

		auto lobbyIndex = pUser->GetLobbyIndex();
		auto pLobby = m_pRefLobbyMgr->GetLobby(lobbyIndex);
		if (pLobby == nullptr) {
			resPkt.SetError(ERROR_CODE::ROOM_MASTER_GAME_START_INVALID_LOBBY_INDEX);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_GAME_START_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_MASTER_GAME_START_INVALID_LOBBY_INDEX;
		}

		auto pRoom = pLobby->GetRoom(pUser->GetRoomIndex());
		if (pRoom == nullptr) {
			resPkt.SetError(ERROR_CODE::ROOM_MASTER_GAME_START_INVALID_ROOM_INDEX);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_GAME_START_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_MASTER_GAME_START_INVALID_ROOM_INDEX;
		}

		// 방의 상태가 게임을 안하는 중인지?
		if (pRoom->GetGameObj()->GetState() != GameState::STARTTING) {
			resPkt.SetError(ERROR_CODE::ROOM_MASTER_GAME_START_INVALID_GAME_STATE);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_GAME_START_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_MASTER_GAME_START_INVALID_GAME_STATE;
		}


		//TODO: 이미 게임 시작 요청을 했는가?

		//TODO: 방에서 게임 시작 요청한 유저 리스트에 등록

		// 방의 다른 유저에게 게임 시작 요청을 했음을 알리고

		// 요청자에게 답변을 보낸다.
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_GAME_START_RES, sizeof(resPkt), (char*)&resPkt);


		// 게임 시작 가능한가?
		// 시작이면 게임 상태 변경 GameState::ING
		// 게임 시작 패킷 보내기
		// 방의 상태 변경 로비에 알리고
		// 게임의 선택 시작 시간 설정
		return ERROR_CODE::NONE;
	}
}