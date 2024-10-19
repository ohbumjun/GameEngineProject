#include "Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Renderer/Renderer2D.h"
#include "hzpch.h"

namespace Hazel
{
Renderer::SceneData *Renderer::m_SceneData = new Renderer::SceneData;

// 역할
// - 우리가 사용하는 모든 shader 가 right uniform 을 가질 수 있게 처리
void Renderer::BeginScene(OrthographicCamera &camera)
{
    m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
}
void Renderer::EndScene()
{
}
void Renderer::Init()
{
    HZ_PROFILE_FUNCTION();

    // ex) openGL 설정값
    RenderCommand::Init();

    // ex) Vertex, Shader 등 세팅
    Renderer2D::Init();
}
void Renderer::ShutDown()
{
    Renderer2D::ShutDown();
}
void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
    // Multi ViewPort 사용시 수정할 사항
    RenderCommand::SetViewPort(0, 0, width, height);
}
void Renderer::Submit(const std::shared_ptr<VertexArray> &vertexArray,
                      const std::shared_ptr<Shader> &shader,
                      const glm::mat4 &transform)
{
    // 실제 draw 하기 전에 bind
    shader->Bind();

    // 한 프레임에서 Shader 당 한번씩만 호출해줘도 된다.
    std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4(
        "u_ViewProjection",
        m_SceneData->ViewProjectionMatrix);

    // 각 object 마다 해당 함수를 호출해줘야 한다.
    std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4(
        "u_Transform",
        transform);

    vertexArray->Bind();

    // 1) RenderCommand Queue 에 push
    // 차후 특정 쓰레드가 해당 Queue 에 있는 Command 들을 처리하게 될 것이다.
    // (즉, 해당 함수를 호출한다고 하여 바로 RenderCommand 를 만들어서 GPU 에 넘겨주는 것이 아니라는 것이다
    // 현재는 즉시 바로 REnder 하지만,
    // 차후에는 하나의 Scene 을 구성하는 모든 요소가 준비될때까지 기다리고
    // 그때 DrawCall 을 때리는 방식으로 진행할 것이다.
    // Culling 도 하고 Batching 도 하고 Sorting 도 다 하고 나서 Drawcall
    // 해야 하는 경우도 있기 때문이다.

    // 2) 멀티쓰레드 렌더링
    // Renderer 를 담당하는 쓰레드가 별도로 존재하게 될 것이다.
    // 즉, application app 이 submit 하는 동안
    // Renderer 는 또 별도로 이전 Frame 의 데이터를 Render 하고 있을 것이다.
    // vsync 개념. 즉, 현재 Frame 에 대한 내용을 application app 이 모두
    // submit 하면, 또 Renderer 쓰레드는 그 Submit 내용을 이용해서 Render
    RenderCommand::DrawIndexed(vertexArray);
}
} // namespace Hazel