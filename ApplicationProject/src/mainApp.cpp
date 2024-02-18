#include <Hazel.h>

/*Entry Point -> Application 프로젝트 한 곳에서만 include 해야 한다.
따라서main app 이 있는 곳으로 include 처리한다.
*/
#include "Hazel/Core/EntryPoint.h"

#include "Platform/OpenGL/OpenGLShader.h"
// 참고 : imgui 가 include 되는 원리 (혹은 link 원리)
// imgui 를 빌드하여 static library로 만든다.
// (imgui -> static lib)
// 그리고 hazel 로 static lib 형태로 include 가 되게 한다.
// hazel 은 dll 형태로 뽑아지게 된다.
// (hazel -> dll)
// client 는 dll 형태의 hazel 을 include 하는 것이다.
#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>

#include "SandBox2D.h"

class ExampleLayer : public Hazel::Layer
{
public: 
	ExampleLayer()
		: Layer("Example"),
		// m_CameraController( -1.6f, 1.6f, -0.9f, 0.9f ),
		m_CameraController(1200.f / 720.f, true)
	{

		// Create Vertex Array
		m_VertexArray = Hazel::VertexArray::Create();

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

		std::shared_ptr<Hazel::VertexBuffer> vertexBuffer;
		vertexBuffer = Hazel::VertexBuffer::Create(vertices, sizeof(vertices));

		Hazel::BufferLayout layout = {
			{Hazel::ShaderDataType::Float3, "a_Position"},
			{Hazel::ShaderDataType::Float4, "a_Color"}
		};

		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		Hazel::Ref<Hazel::IndexBuffer> indexBuffer;
		indexBuffer = Hazel::IndexBuffer::Create(indices, 3);
		m_VertexArray->SetIndexBuffer(indexBuffer);

		/*Square*/
		m_SquareArray = Hazel::VertexArray::Create();

		// 5 floats per each vertex
		/*Vertex Pos + Texture Cordinate*/
		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  /*Bottom Left  */
			0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  /*Bottom Right*/
			0.5f,  0.5f, 0.0f, 1.0f, 1.0f,   /*Top Right*/
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f    /*Top Left*/
		};

		Hazel::Ref<Hazel::VertexBuffer> squareVB;
		squareVB = Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices));

		Hazel::BufferLayout squareVBLayout = {
			{Hazel::ShaderDataType::Float3, "a_Position"},
			{Hazel::ShaderDataType::Float2, "a_TexCoord"}
		};

		squareVB->SetLayout(squareVBLayout);
		m_SquareArray->AddVertexBuffer(squareVB);

		uint32_t squareIndices[] = { 0, 1, 2, 2, 3, 0 };
		Hazel::Ref<Hazel::IndexBuffer> squareIdxB;
		squareIdxB = Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		m_SquareArray->SetIndexBuffer(squareIdxB);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color    = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
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

		m_Shader = Hazel::Shader::Create("vertexColorTriangle",vertexSrc, fragmentSrc);

		std::string sqaureVertexScr = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		std::string sqaureFragSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			uniform vec4 u_Color;

			void main()
			{
				color = u_Color;
			}
		)";

		m_BlueShader = Hazel::Shader::Create("squareBlueShader", sqaureVertexScr, sqaureFragSrc);

		// m_TextureShader.reset(Hazel::Shader::Create(textureShaderVertexSrc, textureShaderFragmentSrc));
		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		// Create Texture
		m_Texture = Hazel::TextureManager::CreateTexture2D("assets/textures/RandomBox.png");
		m_TransparentTexture = Hazel::TextureManager::CreateTexture2D("assets/textures/opaque.png");

		std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);

}

	void OnUpdate(Hazel::Timestep ts) override
	{
		// Update
		m_CameraController.OnUpdate(ts);

		// Render
		Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
		Hazel::RenderCommand::Clear();


		// Renderer::BeginScene(camera, lights, environment);
		// Scene 을 그리기 위해 필요한 모든 것을 한번에 그려낸다.
		Hazel::Renderer::BeginScene(m_CameraController.GetCamera());

		glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(0.1f));

		/*
		Hazel::MaterialRef material = new Hazel::Material(m_FlatColorShader);
		Hazel::MaterialInstanceRef material = new Hazel::MaterialInstance(material);
		material->Set("u_Color", redColor);
		material->Set("u_Texture", texture);
		squareMesh->SetMaterial(material);
		*/

		// 매 프레임마다 넘겨주는 것이다.
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_BlueShader)->Bind();
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_BlueShader)->UploadUniformFloat4("u_Color", m_SquareColor);

		for (int i = 0; i < 5; ++i)
		{
			glm::vec3 pos(i * 0.2f, 0.f, 0.f);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
			Hazel::Renderer::Submit(m_SquareArray, m_BlueShader, transform);
		}

		// Bind Texture
		m_Texture->Bind();
		auto textureShader = m_ShaderLibrary.Get("Texture");
		Hazel::Renderer::Submit(m_SquareArray, textureShader, glm::scale(glm::mat4(1.f), glm::vec3(1.5f)));
		
		m_TransparentTexture->Bind();
		Hazel::Renderer::Submit(m_SquareArray, textureShader, glm::scale(glm::mat4(1.f), glm::vec3(1.5f)));

		// Triangle
		// Hazel::Renderer::Submit(m_VertexArray, m_Shader);

		Hazel::Renderer::EndScene();
	}

	void OnEvent(Hazel::Event& event) override
	{
		m_CameraController.OnEvent(event);
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		
		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
		
		ImGui::End();
	}

private :
	Hazel::ShaderLibrary m_ShaderLibrary;

	Hazel::Ref<Hazel::Shader> m_Shader;
	Hazel::Ref<Hazel::VertexArray> m_VertexArray;

	Hazel::Ref<Hazel::Shader> m_BlueShader;
	Hazel::Ref<Hazel::VertexArray> m_SquareArray;

	Hazel::Ref<Hazel::Texture2D> m_Texture;
	Hazel::Ref<Hazel::Texture2D> m_TransparentTexture;

	Hazel::OrthographicCameraController m_CameraController;
	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.f};
};

class Sandbox : public Hazel::Application
{
public:
	Sandbox(const Hazel::ApplicationSpecification& specification)
		: Hazel::Application(specification)
	{
		// PushLayer(new ExampleLayer());
		PushLayer(new SandBox2D());
		
		// Client 측에서 ImGuiLayer 를 세팅하게 해주고 싶지 않다.
		// Engine 측 Application 에서 추가하게 할 것이다.
		// PushOverlay(new Hazel::ImGuiLayer());


	}

	~Sandbox()
	{

	}

};

Hazel::Application* Hazel::CreateApplication(Hazel::ApplicationCommandLineArgs args)
{
	Hazel::ApplicationSpecification spec;
	spec.Name = "Sandbox";
	spec.WorkingDirectory = "../Hazelnut";
	spec.CommandLineArgs = args;

	return new Sandbox(spec);
}