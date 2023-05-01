#include "hzpch.h"
#include "Application.h"
#include "Hazel/Log.h"

#include <glad/glad.h>

#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer.h"
#include "Input.h"

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

		// 해당 ImGuiLayer 에 대한 소유권이 LayerStack 에 있어야하므로
		// Unique Pointer로 생성하면 안된다.
		// m_ImGuiLayer = std::make_unique<ImGuiLayer>();
		m_ImGuiLayer = new ImGuiLayer();
		
		PushOverlay(m_ImGuiLayer);

		// Create Vertex Array
		m_VertexArray.reset(VertexArray::Create());

		// 아래 위치를 통해 Rendering 을 하면
		// 가운데가 0,0,0 이 된다.
		/*
		layout 에 a_Position 만 있을 경우
		float vertices[3 * 3] = {
			-0.5f, -0.5f, 0.0f, // one point
			0.5f,  -0.5f, 0.f,
			0.0f, 0.5f, 0.0f
		};
		*/
		float vertices[3 * 7] = {
			/*pos*/-0.5f, -0.5f, 0.0f,  /*color*/ 1.0f, 0.0f, 1.0f, 1.0f,
			0.5f,  -0.5f, 0.f,			/*color*/1.0f, 0.0f, 1.0f, 1.0f,
			0.0f, 0.5f, 0.0f,			/*color*/1.0f, 0.0f, 1.0f, 1.0f
		};

		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
		
		BufferLayout layout = {
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"}
		};

		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		uint32_t indices[3] = {0, 1, 2};
		m_IndexBuffer.reset(IndexBuffer::Create(indices, 3));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		/*Square*/
		m_SquareArray.reset(VertexArray::Create());
		
		float squareVertices[3 * 4] = {
			-0.75f, -0.5f, 0.0f,  
			0.75f,  -0.5f, 0.f,		
			0.75f,  0.5f, 0.0f,	
			-0.75f, 0.5f, 0.0f
		};
		
		std::shared_ptr<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		BufferLayout squareVBLayout = {
			{ShaderDataType::Float3, "a_Position"}
		};

		squareVB->SetLayout(squareVBLayout);
		m_SquareArray->AddVertexBuffer(squareVB);

		uint32_t squareIndices[] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<IndexBuffer> squareIdxB;
		squareIdxB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareArray->SetIndexBuffer(squareIdxB);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color    = a_Color;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				// color = vec4(v_Position * 0.5f + 0.5f, 1.0);
				color = v_Color;
			}
		)";

		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));
	
		std::string vertexSrc2 = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc2 = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main()
			{
				color = vec4(0.2f, 0.2, 0.8f, 1.0);
			}
		)";

		m_BlueShader.reset(new Shader(vertexSrc2, fragmentSrc2));

	}
	Application::~Application()
	{
	}
	void Application::Run()
	{
		while (m_Running)
		{
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f});
			RenderCommand::Clear();

			// Renderer::BeginScene(camera, lights, environment);
			// Scene 을 그리기 위해 필요한 모든 것을 한번에 그려낸다.
			Renderer::BeginScene();

			// 실제 draw 하기 전에 bind
			m_BlueShader->Bind();
			Renderer::Submit(m_SquareArray);

			m_Shader->Bind();
			Renderer::Submit(m_VertexArray);

			// Renderer::Flush();
			
			Renderer::EndScene();

			for (Layer* layer : m_LayerStack)
			{
				// ex) submit things for rendering
				layer->OnUpdate();
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

		// 2) Event Dispatcher 에 의해 넘어온 Event 에 맞게
		// 적절한 함수를 binding 시켜줄 것이다.
		HZ_CORE_INFO("{0}", e);

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
};