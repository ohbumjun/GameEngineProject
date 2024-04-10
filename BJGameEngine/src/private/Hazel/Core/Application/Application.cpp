#include "Hazel/Core/Application/Application.h"
#include "Hazel/Input/Input.h"
#include "Hazel/Utils/Log.h"
#include "Hazel/Utils/PlatformUtils.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer.h"
#include "Hazel/Core/Thread/ThreadExecuter.h"
#include "hzpch.h"
#include <glad/glad.h>

namespace Hazel
{
/*
	- std::bind : std::function 객체 리턴
	ex) BIND_EVENT_FN(OnEvent)
		- Application Class 의 OnEvent 함수정보를 담은 std::function 객체를 생성
		- 현재 Application::s_Instance 라는 객체의 멤버함수 실행
		- 해당 std::function 은 인자를 하나 받는 형태가 된다.
	*/
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

// make it as single ton
Application *Application::s_Instance = nullptr;

static ThreadExecuterManager::ThreadHandle *s_MainThreadExecuter = nullptr;

ApplicationCommandLineArgs::ApplicationCommandLineArgs(int count,
                                                              char **Args) : 
    m_Count(count)
{
	for (int i = 0; i < count; i++)
	{
        m_Args.push_back(Args[i]);
	}
}

Application::Application(const ApplicationSpecification &specification)
    : m_Specification(specification)
{
    HZ_PROFILE_FUNCTION();

    HZ_CORE_ASSERT(!s_Instance, "Application Alread Exists");

    s_Instance = this;

    // Set working directory here
    if (!m_Specification.WorkingDirectory.empty())
    {
        // std::filesystem::current_path(m_Specification.WorkingDirectory);
    }

    Initialize();
    Renderer::Init();
}
Application::~Application()
{
    HZ_PROFILE_FUNCTION();

    if (s_MainThreadExecuter)
    {
        delete s_MainThreadExecuter;
    }

    ThreadExecuterManager::Finalize();

    Renderer::ShutDown();
}
void Application::Run()
{
    while (m_Running)
    {
        HZ_PROFILE_SCOPE("Run Loop");

        float time = Time::GetTime(); // Platform::GetTime

        // Frame 사이의 소요시간
        Timestep timeStep = time - m_LastFrameTime;

        m_LastFrameTime = time;

        // 최소화 버튼을 누르고 나면 실질적인 Rendering 을 적용하지 않을 것이다.
        // IMGUI 는 최소화 버튼을 눌러도, 별도로 Docking 되어 진행될 수 있다.
        // 따라서 m_Minimized 변수에 영향 받지 않게 할 것이다.
        if (m_Minimized == false)
        {
            HZ_PROFILE_SCOPE("LayerStack OnUpdate");

            for (Layer *layer : m_LayerStack)
            {
                // ex) submit things for rendering
                layer->OnUpdate(timeStep);
            }
        }

        // IMGUI 관련 세팅 시작
        m_ImGuiLayer->Begin();
        {
            HZ_PROFILE_SCOPE("LayerStack ImguiRender");

            for (Layer *layer : m_LayerStack)
            {
                layer->OnImGuiRender();
            }
        }

        // IMGUI 관련 세팅 끝
        m_ImGuiLayer->End();

        // 해당 줄이 위 줄보다 아래에 와야 한다
        // 아래 함수에 swap buffer 함수가 있어서,
        // Front Buffer 에 그려진 Scene 을 Back Buffer 와 바꿔버리는 역할을 하기 때문이다.
        m_Window->OnUpdate();

        // MainThread 에 예약된 작업을 Frame 마지막에 추가한다.
        s_MainThreadExecuter->ExecuteHandle();
    }
}
void Application::Close()
{
    m_Running = false;
}
void Application::OnEvent(Event &e)
{
    HZ_PROFILE_FUNCTION();

    // 1) Window 에서 Event 를 입력받아 적절한 Event 객체 + EventDispatcher 객체를 만들어서
    // OnEvent 함수를 호출한다.
    EventDispatcher dispatcher(e);

    // e 가 WindowCloseEvent 라면 !! OnWindowClose 함수를 실행하게 한다
    // 만약 아니라면 내부적으로 실행 X
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
    dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

    // 2) Event Dispatcher 에 의해 넘어온 Event 에 맞게
    // 적절한 함수를 binding 시켜줄 것이다.
    // HZ_CORE_INFO("{0}", e);

    // 가장 마지막에 그려지는 Layer 들. 즉, 화면 가장 위쪽에 있는 Layer 들 부터
    // 차례로 이벤트 처리를 한다.
    // 즉, stack 의 뒤에서 앞으로, 위에서 아래로, 순서로 event 를 처리한다.
    for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
    {
        if (e.m_Handled)
            break;
        (*--it)->OnEvent(e);
    }
}
void Application::Finalize()
{
}
void Application::Initialize()
{
    // Window 생성자 호출 => WIndowsWindow 생성
    m_Window = std::unique_ptr<Window>(
        Window::Create(WindowProps(m_Specification.Name)));

    // WindowsWindow.WindowsData.EventCallback 에 해당 함수 세팅
    m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

    s_MainThreadExecuter = ThreadExecuterManager::Initialize();

    // 해당 ImGuiLayer 에 대한 소유권이 LayerStack 에 있어야하므로
    // Unique Pointer로 생성하면 안된다.
    // m_ImGuiLayer = std::make_unique<ImGuiLayer>();
    m_ImGuiLayer = new ImGuiLayer();

    PushOverlay(m_ImGuiLayer);
};
void Application::PushLayer(Layer *layer)
{
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
};
void Application::PushOverlay(Layer *layer)
{
    m_LayerStack.PushOverlay(layer);
    layer->OnAttach();
};
void Application::PopLayer(Layer *layer)
{
    m_LayerStack.PopLayer(layer);
};
bool Application::OnWindowClose(WindowCloseEvent &e)
{
    // 해당 함수 호추 이후, Application::Run( ) 함수가 더이상 돌아가지 않게 될 것이다.
    m_Running = false;
    return true;
};
bool Application::OnWindowResize(WindowResizeEvent &e)
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
}; // namespace Hazel