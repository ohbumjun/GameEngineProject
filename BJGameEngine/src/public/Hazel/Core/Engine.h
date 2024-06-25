#pragma once

#include "EngineContext.h"

namespace Hazel
{

	/*
	* 현재 게임 엔진에서는 Application 이 사실상 Engine 의 역할을 한다.
	* 따라서 Engine Class 가 별도로 존재해야 하는가에 대해서는 고민할 필요가 있다.
	* 
	* 그렇다면, Engine ~ Editor 이런 Level 로 내려서 
	* Bundle Play 에서는 Engine 을, Editor Play 에서는 Editor 를 사용하는 개념.
	* 
	* 그리고 Application 은 이러한 Engine, Editor 를 포괄하는 개념
	* 
	* Engine, Editor 는 각각 별도 Layer 로 세팅할 것이다.
	*/
class Engine
{
public:
    Engine();
	~Engine();

	void Initialize();
	void Finalize();

	EngineContext &GetEngineContext() { return m_EngineContext; }

	static Engine* GetInstance()
	{
		if (s_Instance == nullptr)
		{
			s_Instance = new Engine();
		}

		return s_Instance;
	}

	static void DeleteInstance()
	{
		if (s_Instance != nullptr)
		{
			delete s_Instance;
			s_Instance = nullptr;
		}
	}

private:
    EngineContext m_EngineContext;

	static Engine *s_Instance;
};
}; // namespace Hazel
