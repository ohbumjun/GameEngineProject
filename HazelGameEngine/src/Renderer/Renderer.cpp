#include "hzpch.h"
#include "Renderer.h"

namespace Hazel
{
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	// 역할
	// - 우리가 사용하는 모든 shader 가 right uniform 을 가질 수 있게 처리
	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}
	void Renderer::EndScene()
	{
	}
	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray,
		const std::shared_ptr<Shader>& shader)
	{
		// 실제 draw 하기 전에 bind
		shader->Bind();
		shader->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
		
		vertexArray->Bind();

		// RenderCommand Queue 에 push
		// 차후 특정 쓰레드가 해당 Queue 에 있는 Command 들을 처리하게 될 것이다.
		// (즉, 해당 함수를 호출한다고 하여 바로 RenderCommand 를 만들어서 GPU 에 넘겨주는 것이 아니라는 것이다
		RenderCommand::DrawIndexed(vertexArray);
	}
}