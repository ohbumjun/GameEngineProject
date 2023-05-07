#include "hzpch.h"
#include "Application.h"
#include "Hazel/Core/Log.h"

#include <glad/glad.h>

#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer.h"
#include "Input.h"
#include <GLFW/glfw3.h>

namespace Hazel
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	// make it as single ton
	Application* Application::s_Instance = nullptr;


	Application::Application() 
	{
		HZ_CORE_ASSERT(!s_Instance, "Application Alread Exists");
		s_Instance = this;

		// Window 생성자 호출 => WIndowsWindow 생성
		m_Window = std::unique_ptr<Window>(Window::Create());

		// WindowsWindow.WindowsData.EventCallback 에 해당 함수 세팅
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		Renderer::Init();

		// 해당 ImGuiLayer 에 대한 소유권이 LayerStack 에 있어야하므로
		// Unique Pointer로 생성하면 안된다.
		// m_ImGuiLayer = std::make_unique<ImGuiLayer>();
		m_ImGuiLayer = new ImGuiLayer();
		
		PushOverlay(m_ImGuiLayer);

	}
	Application::~Application()
	{
	}
	void Application::Run()
	{
		while (m_Running)
		{
			float time = glfwGetTime(); // Platform::GetTime

			// Frame 사이의 소요시간
			Timestep timeStep = time - m_LastFrameTime;

			m_LastFrameTime = time;

			// 최소화 버튼을 누르고 나면 실질적인 Rendering 을 적용하지 않을 것이다.
			// IMGUI 는 최소화 버튼을 눌러도, 별도로 Docking 되어 진행될 수 있다.
			// 따라서 m_Minimized 변수에 영향 받지 않게 할 것이다.
			if (m_Minimized == false)
			{
				for (Layer* layer : m_LayerStack)
				{
					// ex) submit things for rendering
					layer->OnUpdate(timeStep);
				}
			}

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
			{
				layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();

			// 해당 줄이 위 줄보다 아래에 와야 한다
			// 아래 함수에 swap buffer 함수가 있어서, 
			// Front Buffer 에 그려진 Scene 을 Back Buffer 와 바꿔버리는 역할을 하기 때문이다. 
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
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		// 2) Event Dispatcher 에 의해 넘어온 Event 에 맞게
		// 적절한 함수를 binding 시켜줄 것이다.
		// HZ_CORE_INFO("{0}", e);

		// 가장 마지막에 그려지는 Layer 들. 즉, 화면 가장 위쪽에 있는 Layer 들 부터
		// 차례로 이벤트 처리를 한다.
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.m_Handled)
				break;
		}
	};
	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	};
	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	};
	void Application::PopLayer(Layer* layer)
	{
		m_LayerStack.PopLayer(layer);
	};
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		// 해당 함수 호추 이후, Application::Run( ) 함수가 더이상 돌아가지 않게 될 것이다.
		m_Running = false;
		return true;
	};
	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		// minimize 
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;

			return false;
		}

		m_Minimized = false;

		// Tell Renderer the change
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		// 모든 Layer 가 Resize Event 를 알게 하기 위해 return false
		return false;
	};
};