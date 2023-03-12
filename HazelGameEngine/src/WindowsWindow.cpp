#include "hzpch.h"
#include "WindowsWindow.h"

namespace Hazel
{
	// Init 과정을 딱 한번만 진행해주기 위해서 static 변수를 활용
	static bool s_GLFWInitialized = false;

	Window* Hazel::Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		// HZ_CORE_INFO("Creating Window {0} {1} {2}", props.Title, props.Width, props.Height);
	
		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			HZ_CORE_ASSERT(success, "Could not initialize GLFW");

			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), 
			nullptr, nullptr);

		// make context current
		glfwMakeContextCurrent(m_Window);

		// m_Data 라는 구조체를 인자로 넘겨서, m_Data.EventCallback 이라는 변수에 데이터 세팅
		glfwSetMonitorUserPointer((GLFWmonitor*)m_Window, &m_Data);

		SetVSync(true);

		// Set GLFW callbacks
		// - window size 바뀔 때마다 호출되는 콜백
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			// window 는 glfwSetMonitorUserPointer 를 통해 세팅한 m_Window 가 들어오게 될 것이다.
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		});
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

	
}

