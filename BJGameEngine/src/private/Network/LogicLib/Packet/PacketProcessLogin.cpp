#include <tuple>

#include "Network/Common/Packet.h"
#include "Network/Common/PacketId.h"
#include "Network/Common/ErrorCode.h"
#include "Network/NetLib/TcpNetwork.h"
#include "Network/LogicLib/User/ConnectedUserManager.h"
#include "Network/LogicLib/User/User.h"
#include "Network/LogicLib/User/UserManager.h"
#include "Network/LogicLib/Lobby/LobbyManager.h"
#include "Network/LogicLib/Lobby/Lobby.h"
#include "Network/LogicLib/Packet/PacketProcess.h"

using PACKET_ID = NCommon::PACKET_ID;

namespace NLogicLib
{
	/*
	 * Client 측에서 로그인 요청이 왔을 때 실행해주는 함수
	 *
	 */
	ERROR_CODE PacketProcess::Login(PacketInfo packetInfo)
	{
		//TODO: 받은 데이터가 PktLogInReq 크기만큼인지 조사해야 한다.
		// 패스워드는 무조건 pass 해준다. -> 이 조건도 변경할 것이다. (DB 연동할 경우)
		// ID 중복이라면 에러 처리한다.

		NCommon::PktLogInRes resPkt;

		// packetInfo.pRefData 은 Client Application 이 보낸 Data 중에서 Packet Header 부분을 제외한
		// Packet Body 부분이 존재한다.
		NCommon::PktLogInReq* reqPkt = (NCommon::PktLogInReq*)packetInfo.pRefData;

		// DB 를 연동할 경우, 여기에 Login 관련 검증 처리를 할 수 있을 것이다.
		NCommon::ERROR_CODE addRet = m_pRefUserMgr->AddUser(packetInfo.SessionIndex, reqPkt->szID);

		if (addRet != ERROR_CODE::NONE) {

			// error 가 발생했다면, error 정보를 전송한다.
			// 이때. resPkt 은 실제 전송하는 패킷의 "데이터" 부분이다.
			resPkt.SetError(addRet);


			// 참고 : 여기서 바로 실제 send 를 호출하지 않는다.
			// 그 다음 network->Run Loop 에서 Send 해준다.
			// 즉, 네트워크 상의 클라이언트에게 패킷을 보내게 된다.
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOGIN_IN_RES, sizeof(NCommon::PktLogInRes), (char*)&resPkt);
			return addRet;
		}

		// 해당 Client Session 에 대해서 Login 처리를 한다.
		m_pConnectedUserManager->SetLogin(packetInfo.SessionIndex);

		resPkt.ErrorCode = (short)addRet;

		// 다시 해당 클라이언트 소켓을 통해 Client Application 측으로 패킷을 보낸다.
		// (참고) 다음 네트워크 루프에서 SendSocket 를 호출하게 된다.
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOGIN_IN_RES, sizeof(NCommon::PktLogInRes), (char*)&resPkt);

		return ERROR_CODE::NONE;
	}

	ERROR_CODE PacketProcess::LobbyList(PacketInfo packetInfo)
	{
		// 인증 받은 유저인가?
		// 아직 로비에 들어가지 않은 유저인가?
		NCommon::PktLobbyListRes resPkt;
		auto [errorCode, pUser] = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);

		if (errorCode != ERROR_CODE::NONE) {
			// 1) 유저가 없다.
			// 2) Confirm 상태가 아니다.
			resPkt.SetError(errorCode);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_LIST_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		if (pUser->IsCurDomainInLogIn() == false) 
		{
			// User 가 Login 상태가 아니다.
			resPkt.SetError(ERROR_CODE::LOBBY_LIST_INVALID_DOMAIN);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_LIST_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::LOBBY_LIST_INVALID_DOMAIN;
		}

		m_pRefLobbyMgr->SendLobbyListInfo(packetInfo.SessionIndex);

		return ERROR_CODE::NONE;
	}
}