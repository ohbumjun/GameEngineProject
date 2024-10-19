#include "Network/LogicLib/User/ConnectedUserManager.h"

namespace NLogicLib
{
	void ConnectedUserManager::Init(const int maxSessionCount, TcpNet* pNetwork, 
		NServerNetLib::ServerConfig* pConfig, ILog* pLogger)
	{
		m_pRefLogger = pLogger;
		m_pRefNetwork = pNetwork;
		
		// ex) maxsessioncount : TCPNetwork 측에서 세팅한 최대 Client 세션 개수
		for (int i = 0; i < maxSessionCount; ++i)
		{
			ConnectedUserList.emplace_back(ConnectedUser());
		}

		m_IsLoginCheck = pConfig->IsLoginCheck;
	}
	void ConnectedUserManager::SetConnectSession(const int sessionIndex)
	{
		/*
		>> time(&ConnectedUserList[sessionIndex].m_ConnectedTime);: 
			This line calls the time function from the time.h library. 
			The time function gets the current system time in seconds since the epoch 
			(January 1, 1970, 00:00:00 UTC). 
			The result is stored in the memory location pointed to by the argument.

			즉, ConnectedUserList[sessionIndex].m_ConnectedTime 에 현재 시간 정보를 저장한다.
		*/
		time(&ConnectedUserList[sessionIndex].m_ConnectedTime);
	}
	void ConnectedUserManager::SetLogin(const int sessionIndex)
	{
		ConnectedUserList[sessionIndex].m_IsLoginSuccess = true;
	}
	void ConnectedUserManager::SetDisConnectSession(const int sessionIndex)
	{
		ConnectedUserList[sessionIndex].Clear();
	}
	void ConnectedUserManager::LoginCheck()
	{
		if (m_IsLoginCheck == false) {
			return;
		}	
		
		auto curTime = std::chrono::system_clock::now();
		auto diffTime = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - m_LatestLoginCheckTime);

		// 60밀리초 마다 검사
		if (diffTime.count() < 60)
		{
			return;
		}
		else
		{
			// 60 mili 초 마다 logic check 시간을 검사한다.
			m_LatestLoginCheckTime = curTime;
		}

		auto curSecTime = std::chrono::system_clock::to_time_t(curTime);

		const auto maxSessionCount = (int)ConnectedUserList.size();

		if (m_LatestLogincheckIndex >= maxSessionCount) {
			m_LatestLogincheckIndex = -1;
		}

		// 음...User 하나하나 idx 를 증가시켜가면서 검사한다는 의미인가 ?
		++m_LatestLogincheckIndex;

		// ? 100 은 왜 또 더하는 거지 ?
		// -> LoginCheck 한번의 실행 마다 100개 User ? 에 대해서 Login 여부를
		// 계속해서 검사하는 것으로 보인다.
		auto lastCheckIndex = m_LatestLogincheckIndex + 100;
		if (lastCheckIndex > maxSessionCount) {
			lastCheckIndex = maxSessionCount;
		}

		for (; m_LatestLogincheckIndex < lastCheckIndex; ++m_LatestLogincheckIndex)
		{
			auto i = m_LatestLogincheckIndex;

			if (ConnectedUserList[i].m_ConnectedTime == 0 ||
				ConnectedUserList[i].m_IsLoginSuccess == false)
			{
				// 애초에 login 된 상태가 아닌 User 정보였다면 건너뛴다.
				continue;
			}

			auto diff = curSecTime - ConnectedUserList[i].m_ConnectedTime;

			if (diff >= 180)
			{
				// 일정 시간 이상 동안 Login check 가 안된 User 라면 강제로 접속을 끊는다.
				m_pRefLogger->Write(NServerNetLib::LOG_TYPE::L_WARN, "%s | Login Wait Time Over. sessionIndex(%d).", __FUNCTION__, i);
				m_pRefNetwork->ForcingClose(i);
			}
		}
	}
}
