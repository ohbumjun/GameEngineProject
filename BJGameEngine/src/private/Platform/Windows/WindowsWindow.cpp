#include "hzpch.h"
#include "Platform/Windows/WindowsWindow.h"
#include "Hazel/Event/MouseEvent.h"
#include "Hazel/Event/ApplicationEvent.h"
#include "Hazel/Event/KeyEvent.h"

#include <glad/glad.h>

#include "Platform/OpenGL/OpenGLContext.h"

namespace Hazel
{
	// Init 과정을 딱 한번만 진행해주기 위해서 static 변수를 활용
	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* desc)
	{
		HZ_CORE_ERROR("GLFW Error {{0}} : {1}", error, desc);
	}

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

		// refresh window 
		// 왜 m_Context->SwapBuffers() 안에 해당 함수를 옮겨놓는가 ?
		// 여러 render api 에 맞게 동작시키기 위해서 일종의 Renderer 라는 Wrapper 로
		// 한번 더 감싸는 것이다. 아래의 함수는 open gl 이라는 render api 에만
		// 한정되어 동작하는 함수이기 때문이다.
		// glfwSwapBuffers(m_Window);

		m_Context->SwapBuffers();

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
		HZ_PROFILE_FUNCTION();

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		// HZ_CORE_INFO("Creating Window {0} {1} {2}", props.Title, props.Width, props.Height);

		if (!s_GLFWInitialized)
		{
			HZ_PROFILE_SCOPE("glfwInit");
			int success = glfwInit();
			HZ_CORE_ASSERT(success, "Could not initialize GLFW");

			// set error callback
			glfwSetErrorCallback(GLFWErrorCallback);

			s_GLFWInitialized = true;
		}

		{
			HZ_PROFILE_SCOPE("glfwCreateWindow");
			
			m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(),
				nullptr, nullptr);
		}

		m_hInstance = GetModuleHandle(NULL);

		m_Context = new OpenGLContext(m_Window);
		m_Context->Init();

		// m_Data 라는 구조체를 인자로 넘겨서, m_Data.EventCallback 이라는 변수에 데이터 세팅
		glfwSetWindowUserPointer(m_Window, &m_Data);

		SetVSync(true);

		// Set GLFW callbacks
		// - window size 바뀔 때마다 호출되는 콜백
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				// window 는 glfwSetMonitorUserPointer 를 통해 세팅한 m_Window 가 들어오게 될 것이다.
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				WindowResizeEvent event(width, height);
				data.Width = width;
				data.Height = height;

				// 1) Resilze 가 일어날 때마다 
				// 2) Hazel 자체의 Event Class 형태로 이벤트를 변화시키고
				// 3) EventCallback(event) 라는 함수를 호출한다.
				// 4) 이를 통해 Application.OnEvent 함수를 호출한다.

				// Q. EventCallback == Application::OnEvent 는 언제 세팅된 거임 ?
				// - Application 생성자에서 호출
				data.EventCallback(event);
			});

		// 닫을 때 호출되는 콜백
		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				// 인자로 들어오는 key 들은 openGL에 국한된 코드
				// Hazel KeyCode 로 바꿔서, 다른 Engine 에도 범용적으로 적용될 수 있게 해야 한다.
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, true);
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int character)
			{
				// 인자로 들어오는 key 들은 openGL에 국한된 코드
				// Hazel KeyCode 로 바꿔서, 다른 Engine 에도 범용적으로 적용될 수 있게 해야 한다.
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(character);
				data.EventCallback(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

	
}

