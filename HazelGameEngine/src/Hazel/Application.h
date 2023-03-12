#pragma once

#include "Core.h"
#include "Event/Event.h"
#include "Window.h"

// Application 의 실행을 Application Project 가 아니라
// Engine 단에서 제어하고 싶은 것
namespace Hazel
{
	// _declspec(dllexport) class Application
	class HAZEL_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();
	private :
		// unique ptr 로 세팅해줌으로써 소멸자에서 별도로 소멸시켜줄 필요가 없다.
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;;
	};

	// To be defined in Client 
	Application* CreateApplication();
};

