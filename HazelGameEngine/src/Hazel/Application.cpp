#include "hzpch.h"
#include "Application.h"
#include "Hazel/Log.h"

#include <GLFW/glfw3.h>

namespace Hazel
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application::Application()
	{
		// Window 생성자 호출 => WIndowsWindow 생성
		m_Window = std::unique_ptr<Window>(Window::Create());

		// WindowsWindow.WindowsData.EventCallback 에 해당 함수 세팅
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
	}
	Application::~Application()
	{
	}
	void Application::Run()
	{
		while (m_Running)
		{
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
		}
	}
	void Application::OnEvent(Event& e)
	{
		// 1) Window 에서 Event 를 입력받아 적절한 Event 객체 + EventDispatcher 객체를 만들어서
		// OnEvent 함수를 호출한다.
		EventDispatcher dispatcher(e);

		// e 가 WindowCloseEvent 라면 !! OnWindowClose 함수를 실행하게 한다
		// 만약 아니라면 실행 X
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		// 2) Event Dispatcher 에 의해 넘어온 Event 에 맞게
		// 적절한 함수를 binding 시켜줄 것이다.
		HZ_CORE_INFO("{0}", e);
	}
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		// 해당 함수 호추 이후, Application::Run( ) 함수가 더이상 돌아가지 않게 될 것이다.
		m_Running = false;
		return true;
	}
	Application* CreateApplication()
	{
		return nullptr;
	}
};