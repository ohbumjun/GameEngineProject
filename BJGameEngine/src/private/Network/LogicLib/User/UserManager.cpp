#include <algorithm>
#include "Network/Common/ErrorCode.h"
#include "Network/LogicLib/User/User.h"
#include "Network/LogicLib/User/UserManager.h"

namespace NLogicLib
{
	UserManager::UserManager()
	{
	}

	UserManager::~UserManager()
	{
	}

	void UserManager::Init(const int maxUserCount)
	{
		// 로그인이 될 때마다 매번 User 객체를 새로 만들어줄 필요가 없다.
		// 어차피 총 몇명의 User 동시 접속을 받을 지를 알기 때문이다.
		// 미리 객체 풀을 만들고
		// 사용 o, 사용 x 를 구분해주기 위해서
		// 사용하지 않는 객체의 idx 번호를 m_UserObjPoolIndex 에 넣어주는 것이다.
		for (int i = 0; i < maxUserCount; ++i)
		{
			User user;
			user.Init((short)i);

			m_UserObjPool.push_back(user);
			m_UserObjPoolIndex.push_back(i);
		}
	}

	User* UserManager::AllocUserObjPoolIndex()
	{
		if (m_UserObjPoolIndex.empty()) {
			return nullptr;
		}

		int index = m_UserObjPoolIndex.front();
		m_UserObjPoolIndex.pop_front();
		return &m_UserObjPool[index];
	}

	void UserManager::ReleaseUserObjPoolIndex(const int index)
	{
		m_UserObjPoolIndex.push_back(index);
		m_UserObjPool[index].Clear();
	}

	NCommon::ERROR_CODE UserManager::AddUser(const int sessionIndex, const char* pszID)
	{
		if (FindUser(pszID) != nullptr) {
			return NCommon::ERROR_CODE::USER_MGR_ID_DUPLICATION;
		}

		// 사용가능한 ? User 객체 정보를 가져온다.
		User* pUser = AllocUserObjPoolIndex();

		// nullptr 이라는 것은, 사용할 수 있는 pool 이 더 이상 없다는 것이다.
		if (pUser == nullptr)
		{
			return NCommon::ERROR_CODE::USER_MGR_MAX_USER_COUNT;
		}

		pUser->Set(sessionIndex, pszID);

		m_UserSessionDic.insert({ sessionIndex, pUser });
		m_UserIDDic.insert({ pszID, pUser });

		return NCommon::ERROR_CODE::NONE;
	}

	NCommon::ERROR_CODE UserManager::RemoveUser(const int sessionIndex)
	{
		auto pUser = FindUser(sessionIndex);

		if (pUser == nullptr) {
			return NCommon::ERROR_CODE::USER_MGR_REMOVE_INVALID_SESSION;
		}

		auto index = pUser->GetIndex();
		auto pszID = pUser->GetID();

		m_UserSessionDic.erase(sessionIndex);
		m_UserIDDic.erase(pszID.c_str());
		ReleaseUserObjPoolIndex(index);

		return NCommon::ERROR_CODE::NONE;
	}

	std::tuple<NCommon::ERROR_CODE, User*> UserManager::GetUser(const int sessionIndex)
	{
		auto pUser = FindUser(sessionIndex);

		if (pUser == nullptr) {
			return { NCommon::ERROR_CODE::USER_MGR_INVALID_SESSION_INDEX, nullptr };
		}

		if (pUser->IsConfirm() == false) {
			return{ NCommon::ERROR_CODE::USER_MGR_NOT_CONFIRM_USER, nullptr };
		}

		return{ NCommon::ERROR_CODE::NONE, pUser };
	}

	User* UserManager::FindUser(const int sessionIndex)
	{
		auto findIter = m_UserSessionDic.find(sessionIndex);

		if (findIter == m_UserSessionDic.end()) {
			return nullptr;
		}

		return (User*)findIter->second;
	}

	User* UserManager::FindUser(const char* pszID)
	{
		auto findIter = m_UserIDDic.find(pszID);

		if (findIter == m_UserIDDic.end()) {
			return nullptr;
		}

		return (User*)findIter->second;
	}
}