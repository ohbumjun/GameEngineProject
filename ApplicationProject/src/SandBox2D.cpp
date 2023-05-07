#include "SandBox2D.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>

SandBox2D::SandBox2D()
	: Layer("SandBox2D"),
		m_CameraController(1200.f / 720.f, true)
{
}

void SandBox2D::OnAttach()
{
	/*Square*/
	m_SquareArray = Hazel::VertexArray::Create();

	// 5 floats per each vertex
	/*Vertex Pos + Texture Cordinate*/

	float squareVertices[5 * 4] = {
		-0.5f, -0.5f, 0.0f,  
		0.5f, -0.5f, 0.0f, 
		0.5f,  0.5f, 0.0f,   
		-0.5f,  0.5f, 0.0f,  
	};

	Hazel::Ref<Hazel::VertexBuffer> squareVB;
	squareVB= Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices));

	Hazel::BufferLayout squareVBLayout = {
		{Hazel::ShaderDataType::Float3, "a_Position"}
	};

	squareVB->SetLayout(squareVBLayout);
	m_SquareArray->AddVertexBuffer(squareVB);

	uint32_t squareIndices[] = { 0, 1, 2, 2, 3, 0 };
	Hazel::Ref<Hazel::IndexBuffer> squareIdxB;
	squareIdxB = Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
	m_SquareArray->SetIndexBuffer(squareIdxB);

	m_FlatShader = Hazel::Shader::Create("assets/shaders/FlatColor.glsl");
}

void SandBox2D::OnDetach()
{
}

void SandBox2D::OnUpdate(Hazel::Timestep ts)
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

	// 매 프레임마다 넘겨주는 것이다.
	std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatShader)->Bind();
	std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatShader)->UploadUniformFloat4("u_Color", m_SquareColor);

	Hazel::Renderer::Submit(m_SquareArray, m_FlatShader, glm::scale(glm::mat4(1.f), glm::vec3(1.5f)));

	Hazel::Renderer::EndScene();
}

void SandBox2D::OnEvent(Hazel::Event& event)
{

	m_CameraController.OnEvent(event);
}

void SandBox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");

	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	ImGui::End();
}
