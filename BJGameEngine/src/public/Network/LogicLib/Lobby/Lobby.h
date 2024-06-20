#pragma once

#include <vector>
#include <unordered_map>

namespace NServerNetLib
{
	class ITcpNetwork;
}

namespace NServerNetLib
{
	class ILog;
}

namespace NCommon
{
	enum class ERROR_CODE :short;
}

using ERROR_CODE = NCommon::ERROR_CODE;

namespace NLogicLib
{
	using TcpNet = NServerNetLib::ITcpNetwork;
	using ILog = NServerNetLib::ILog;

	class User;
	class Room;

	// 굳이 LobbyUser 라는 구조체를 만든 이유는
	// LobbyUser 는 일종의 "그릇"
	// Init 에서 Max User 수만큼 해당 구조체 정보를 만들게 된다.
	struct LobbyUser
	{
		short Index = 0;
		User* pUser = nullptr;
	};

	class Lobby
	{
	public:
		Lobby();
		virtual ~Lobby();

		void Init(const short lobbyIndex, const short maxLobbyUserCount, const short maxRoomCountByLobby, const short maxRoomUserCount);

		void Release();

		void SetNetwork(TcpNet* pNetwork, ILog* pLogger);

		short GetIndex() { return m_LobbyIndex; }


		ERROR_CODE EnterUser(User* pUser);
		ERROR_CODE LeaveUser(const int userIndex);

		short GetUserCount();


		Room* CreateRoom();

		Room* GetRoom(const short roomIndex);

		auto MaxUserCount() { return (short)m_MaxUserCount; }

		auto MaxRoomCount() { return (short)m_RoomList.size(); }


	protected:
		void SendToAllUser(const short packetId, const short dataSize, char* pData, const int passUserindex = -1);


	protected:
		User* FindUser(const int userIndex);

		ERROR_CODE AddUser(User* pUser);

		void RemoveUser(const int userIndex);

	protected :
		ILog* m_pRefLogger;
		TcpNet* m_pRefNetwork;

		// 여러 개의 Lobby 가 존재할 수 있다.
		// 몇번째 Lobby 인가
		short m_LobbyIndex = 0;
		short m_MaxUserCount = 0;

		std::vector<LobbyUser> m_UserList;

		// User Index ~ User Map
		std::unordered_map<int, User*> m_UserIndexDic;

		// ID ~ User Map
		std::unordered_map<const char*, User*> m_UserIDDic;

		// 하나의 Lobby 에 여러 Room  이 들어있는 구조인가 ?
		std::vector<Room*> m_RoomList;
	};
};