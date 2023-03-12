#pragma once

#include "Core.h"
#include "Event/Event.h"

// Application 의 실행을 Application Project 가 아니라
// Engine 단에서 제어하고 싶은 것
namespace Hazel
{
	// _declspec(dllexport) class Application
	class HAZEL_API Application
	{
	public:
		virtual ~Application();
		void Run();

	};

	// To be defined in Client 
	Application* CreateApplication();
};

