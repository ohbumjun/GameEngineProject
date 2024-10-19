#include <algorithm>
#include <cstring>
#include <wchar.h>

#include "Network/LogicLib/Game/Game.h"
#include "Network/NetLib/ILog.h"
#include "Network/NetLib/TcpNetwork.h"
#include "Network/Common/Packet.h"
#include "Network/Common/ErrorCode.h"
#include "Network/LogicLib/User/User.h"
#include "Network/LogicLib/Room/Room.h"


namespace NLogicLib
{
	using PACKET_ID = NCommon::PACKET_ID;

	Room::Room() {}

	Room::~Room()
	{
		if (m_pGame != nullptr) {
			delete m_pGame;
		}
	}

	void Room::Init(const short index, const short maxUserCount)
	{
		m_Index = index;
		m_MaxUserCount = maxUserCount;
		m_pGame = new Game;
	}

	void Room::SetNetwork(TcpNet* pNetwork, ILog* pLogger)
	{
		m_pRefLogger = pLogger;
		m_pRefNetwork = pNetwork;
	}

	void Room::Clear()
	{
		m_IsUsed = false;
		m_Title = L"";
		m_UserList.clear();
	}

	NCommon::ERROR_CODE Room::CreateRoom(const wchar_t* pRoomTitle)
	{
		if (m_IsUsed) {
			return NCommon::ERROR_CODE::ROOM_ENTER_CREATE_FAIL;
		}

		m_IsUsed = true;
		m_Title = pRoomTitle;

		return NCommon::ERROR_CODE::NONE;
	}

	NCommon::ERROR_CODE Room::EnterUser(User* pUser)
	{
		if (m_IsUsed == false) {
			return NCommon::ERROR_CODE::ROOM_ENTER_NOT_CREATED;
		}

		if (m_UserList.size() == m_MaxUserCount) {
			return NCommon::ERROR_CODE::ROOM_ENTER_MEMBER_FULL;
		}

		// 중복 검사를 안하는 건가 ?
		m_UserList.push_back(pUser);

		return NCommon::ERROR_CODE::NONE;
	}

	NCommon::ERROR_CODE Room::LeaveUser(const short userIndex)
	{
		if (m_IsUsed == false) {
			return NCommon::ERROR_CODE::ROOM_ENTER_NOT_CREATED;
		}

		auto iter = std::find_if(std::begin(m_UserList), std::end(m_UserList), 
			[userIndex](auto pUser) 
			{ 
				return pUser->GetIndex() == userIndex; 
			}
		);

		if (iter == std::end(m_UserList)) {
			return NCommon::ERROR_CODE::ROOM_LEAVE_NOT_MEMBER;
		}

		m_UserList.erase(iter);

		if (m_UserList.empty())
		{
			Clear();
		}

		return NCommon::ERROR_CODE::NONE;
	}

	bool Room::IsMaster(const short userIndex)
	{
		return m_UserList[0]->GetIndex() == userIndex ? true : false;
	}

	Game* Room::GetGameObj()
	{
		return m_pGame;
	}

	void Room::Update()
	{
		if (m_pGame->GetState() == GameState::ING)
		{
			if (m_pGame->CheckSelectTime())
			{
				//선택 안하는 사람이 지도록 한
			}
		}
	}

	void Room::SendToAllUser(const short packetId, const short dataSize, char* pData, const int passUserindex)
	{
		// Room 에 있는 모든 User 에게 packet 전송
		for (auto pUser : m_UserList)
		{
			if (pUser->GetIndex() == passUserindex) {
				continue;
			}

			m_pRefNetwork->SendData(pUser->GetSessioIndex(), packetId, dataSize, pData);
		}
	}

	void Room::NotifyEnterUserInfo(const int userIndex, const char* pszUserID)
	{
		NCommon::PktRoomEnterUserInfoNtf pkt;
#ifdef _WIN32
		strncpy_s(pkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), pszUserID, NCommon::MAX_USER_ID_SIZE);
#else
		std::strncpy(pkt.UserID, pszUserID, NCommon::MAX_USER_ID_SIZE);
#endif
		// 새로 들어온 User 정보를 모든 User 에게 전송
		SendToAllUser((short)PACKET_ID::ROOM_ENTER_NEW_USER_NTF, sizeof(pkt), (char*)&pkt, userIndex);
	}

	void Room::NotifyLeaveUserInfo(const char* pszUserID)
	{
		if (m_IsUsed == false) {
			return;
		}

		NCommon::PktRoomLeaveUserInfoNtf pkt;
#ifdef _WIN32
		strncpy_s(pkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), pszUserID, NCommon::MAX_USER_ID_SIZE);
#else
		std::strncpy(pkt.UserID, pszUserID, NCommon::MAX_USER_ID_SIZE);
#endif
		SendToAllUser((short)PACKET_ID::ROOM_LEAVE_USER_NTF, sizeof(pkt), (char*)&pkt);

	}

	void Room::NotifyChat(const int sessionIndex, const char* pszUserID, const wchar_t* pszMsg)
	{
		NCommon::PktRoomChatNtf pkt;
#ifdef _WIN32
		// copies the user ID (pszUserID) into the UserID member of the pkt variable.
		strncpy_s(pkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), pszUserID, NCommon::MAX_USER_ID_SIZE);
		// copies the chat message (pszMsg) into the Msg member of the pkt variable. Again, wcsncpy_s is used for safe wide character string copy
		wcsncpy_s(pkt.Msg, NCommon::MAX_ROOM_CHAT_MSG_SIZE + 1, pszMsg, NCommon::MAX_ROOM_CHAT_MSG_SIZE);
#else
		std::strncpy(pkt.UserID, pszUserID, NCommon::MAX_USER_ID_SIZE);
		std::wcsncpy(pkt.Msg, pszMsg, NCommon::MAX_ROOM_CHAT_MSG_SIZE);
#endif
		SendToAllUser((short)PACKET_ID::ROOM_CHAT_NTF, sizeof(pkt), (char*)&pkt, sessionIndex);

	}
}