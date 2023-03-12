#include "hzpch.h"
#include "Application.h"
#include "Hazel/Event/ApplicationEvent.h"
#include "Hazel/Log.h"

namespace Hazel
{
	Application::Application()
	{
		// Window 생성자 호출
		m_Window = std::unique_ptr<Window>(Window::Create());
	}
	Application::~Application()
	{
	}
	void Application::Run()
	{
		// printf("Hello World");

		while (m_Running)
		{
			m_Window->OnUpdate();
		}
	}
	Application* CreateApplication()
	{
		return nullptr;
	}
};