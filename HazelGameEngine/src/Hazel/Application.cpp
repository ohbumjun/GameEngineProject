#include "hzpch.h"
#include "Application.h"

// Event Test
#include "Hazel/Event/ApplicationEvent.h"
#include "Hazel/Log.h"

namespace Hazel
{
	Application::~Application()
	{
	}
	void Application::Run()
	{
		// printf("Hello World");

		WindowResizeEvent e(1200, 720);

		if (e.IsInCategory(EventCategoryApplication))
			HZ_TRACE(e);

		if (e.IsInCategory(EventCategoryInput))
			HZ_TRACE(e);

		while (true);
	}
	Application* CreateApplication()
	{
		return nullptr;
	}
};