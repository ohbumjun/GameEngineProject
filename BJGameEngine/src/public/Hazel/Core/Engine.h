#pragma once

#include "EngineContext.h"

namespace Hazel
{

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
