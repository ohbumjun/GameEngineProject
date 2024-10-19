#pragma once
#include <string>

namespace NLogicLib
{
	class User
	{
	public:
		enum class DOMAIN_STATE {
			NONE = 0,
			LOGIN = 1,
			LOBBY = 2,
			ROOM = 3,
		};

	public:
		User() {}
		virtual ~User() {}

		void Init(const short index)
		{
			m_Index = index;
		}

		// ex) UserManager::RemoveUser -> UserManager::ReleaseUserObjPoolIndex
		void Clear()
		{
			m_SessionIndex = 0;
			m_ID = "";
			m_IsConfirm = false;
			m_CurDomainState = DOMAIN_STATE::NONE;
			m_LobbyIndex = -1;
			m_RoomIndex = -1;
		}

		// ex) UserManager::AddUser
		void Set(const int sessionIndex, const char* pszID)
		{
			m_IsConfirm = true;
			m_CurDomainState = DOMAIN_STATE::LOGIN;

			m_SessionIndex = sessionIndex;
			m_ID = pszID;

		}

		short GetIndex() { return m_Index; }

		int GetSessioIndex() { return m_SessionIndex; }

		std::string& GetID() { return m_ID; }

		bool IsConfirm() { return m_IsConfirm; }

		short GetLobbyIndex() { return m_LobbyIndex; }
		short GetRoomIndex() { return m_RoomIndex; }

		void EnterLobby(const short lobbyIndex)
		{
			m_LobbyIndex = lobbyIndex;
			m_CurDomainState = DOMAIN_STATE::LOBBY;
		}

		void LeaveLobby()
		{
			m_CurDomainState = DOMAIN_STATE::LOGIN;
		}

		void EnterRoom(const short lobbyIndex, const short roomIndex)
		{
			m_LobbyIndex = lobbyIndex;
			m_RoomIndex = roomIndex;
			m_CurDomainState = DOMAIN_STATE::ROOM;
		}

		bool IsCurDomainInLogIn() {
			return m_CurDomainState == DOMAIN_STATE::LOGIN ? true : false;
		}

		// 현재 Room 에 있는지 체크
		bool IsCurDomainInRoom() {
			return m_CurDomainState == DOMAIN_STATE::ROOM ? true : false;
		}

		// 현재 Lobby 에 있는지 체크
		bool IsCurDomainInLobby() {
			return m_CurDomainState == DOMAIN_STATE::LOBBY ? true : false;
		}

	protected:
		// UserManager 의 m_UserObjPoolIndex
		short m_Index = -1;

		// 대응되는 Client Session Index
		int m_SessionIndex = -1;

		// User 가 입력하는 ID
		std::string m_ID;

		// 이 confirm 이라는게 뭐지 ?
		// 로그인 된 상태를 의미하는건가 ?
		bool m_IsConfirm = false;

		// 상태
		// ex) 로그인 상태인지, 로그아웃 상태인지
		DOMAIN_STATE m_CurDomainState = DOMAIN_STATE::NONE;

		// 들어간 Lobby Index
		short m_LobbyIndex = -1;

		// 들어간 방의 idx
		short m_RoomIndex = -1;
	};
}