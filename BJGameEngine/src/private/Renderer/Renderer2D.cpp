#include "Renderer/Renderer2D.h"
#include "Hazel/Asset/Shader/Shader.h"
#include "Renderer/Buffer/UniformBuffer.h"
#include "Renderer/Buffer/VertexArray.h"
#include "Renderer/RenderCommand.h"
#include "hzpch.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STATISTICS 1

namespace Hazel
{
// 각 정점이 가지고 있어야할 정보
struct QuadVertex
{
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 TexCoord;
    float TexIndex; // Texture Slot 상 mapping 된 index
    float TilingFactor;

    // Editor-only
    int EntityID;
};

struct CircleVertex
{
    glm::vec3 WorldPosition;
    glm::vec3 LocalPosition;
    glm::vec4 Color;
    float Thickness;
    float Fade;

    // Editor-only
    int EntityID;
};

struct LineVertex
{
    glm::vec3 Position;
    glm::vec4 Color;

    // Editor-only
    int EntityID;
};

struct Renderer2DData
{
    static const uint32_t MaxQuads = 10000;
    static const uint32_t MaxVertices = MaxQuads * 4;
    static const uint32_t MaxIndices = MaxQuads * 6;
    static const uint32_t MaxTextureSlots = 32;

    Ref<VertexArray> QuadVertexArray;
    Ref<VertexBuffer> QuadVertexBuffer;
    Ref<Shader> QuadShader;
    Ref<Texture2D> WhiteTexture;

    // 총 몇개의 quad indice 가 그려지고 있는가
    // Quad 를 그릴 때마다 + 6 해줄 것이다.
    uint32_t QuadIndexCount = 0;

    // QuadVertex 들을 담은 배열.을 가리키는 포인터
    QuadVertex *QuadVertexBufferBase = nullptr;

    // QuadVertexBufferBase 라는 배열 내에서 각 원소를 순회하기 위한 포인터
    QuadVertex *QuadVertexBufferPtr = nullptr;

    // Cicle
    Ref<VertexArray> CircleVertexArray;
    Ref<VertexBuffer> CircleVertexBuffer;
    Ref<Shader> CircleShader;

    uint32_t CircleIndexCount = 0;
    CircleVertex *CircleVertexBufferBase = nullptr;
    CircleVertex *CircleVertexBufferPtr = nullptr;

    // Line

    Ref<VertexArray> LineVertexArray;
    Ref<VertexBuffer> LineVertexBuffer;
    Ref<Shader> LineShader;


    uint32_t LineVertexCount = 0;
    LineVertex *LineVertexBufferBase = nullptr;
    LineVertex *LineVertexBufferPtr = nullptr;

    float LineWidth = 2.0f;


    std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
    uint32_t TextureSlotIndex = 1; // 0 : Default White Texture 로 세팅

    // mesh local pos
    glm::vec4 QuadVertexPositions[4];

    Renderer2D::Statistics stats;

    struct CameraData
    {
        glm::mat4 ViewProjection;
    };
    CameraData CameraBuffer;
    Ref<UniformBuffer> CameraUniformBuffer;
};

static Renderer2DData s_Data;

void Renderer2D::Init()
{
    HZ_PROFILE_FUNCTION();

    initQuadVertexInfo();
    initCircleVertexInfo();
    initLineVertexInfo();

    initTextures();
    initShaders();
    initUniforms();
}

void Renderer2D::ShutDown()
{
    HZ_PROFILE_FUNCTION();

    delete s_Data.QuadVertexBufferBase;

    s_Data.TextureSlotIndex = 1;
}

void Renderer2D::BeginScene(const Camera &camera, const glm::mat4 &transform)
{
    HZ_PROFILE_FUNCTION();

    // transform : camera local -> world 변환 행렬
    // inverse     : camera world -> local == view
    // const glm::mat4& viewProj = camera.GetProjection() * glm::inverse(transform);
    // s_Data.TextureShader->Bind();
    // s_Data.TextureShader->SetMat4("u_ViewProjection", viewProj);

    s_Data.CameraBuffer.ViewProjection =
        camera.GetProjection() * glm::inverse(transform);
    s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer,
                                        sizeof(Renderer2DData::CameraData));

    startBatch();
}

void Renderer2D::BeginScene(const OrthographicCamera &camera)
{
    HZ_PROFILE_FUNCTION();

    s_Data.QuadShader->Bind();
    s_Data.QuadShader->SetMat4(
        "u_ViewProjection",
        const_cast<OrthographicCamera &>(camera).GetViewProjectionMatrix());

    startBatch();
}

void Renderer2D::BeginScene(const EditorCamera &camera)
{
    HZ_PROFILE_FUNCTION();

    // glm::mat4 viewProj = camera.GetViewProjection();
    // s_Data.TextureShader->Bind();
    // s_Data.TextureShader->SetMat4("u_ViewProjection", viewProj);

    // Uniform Buffer 메모리 할당 + Scene 전체에서 모든 Shader 에도 동일하게 적용되는
    // 값들을 Uniform Buffer 에 할당하게 되는 것이다.
    s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
    s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer,
                                        sizeof(Renderer2DData::CameraData));

    startBatch();
}


void Renderer2D::EndScene()
{
    HZ_PROFILE_FUNCTION();

    Flush();
}

void Renderer2D::FlushAndReset()
{
    EndScene();

    // 1) Begin Scene 과 달리 TextureShader 를 새로 Bind 할 필요도 없고
    // 2) VewProjectionMatrix 를 Bind 할 필요도 없다.

    s_Data.QuadIndexCount = 0;
    s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
    s_Data.TextureSlotIndex = 1;
}

void Renderer2D::initCircleVertexInfo()
{
    // Circle
    s_Data.CircleVertexArray = VertexArray::Create();

    s_Data.CircleVertexBuffer =
        VertexBuffer::Create(s_Data.MaxVertices * sizeof(CircleVertex));
    s_Data.CircleVertexBuffer->SetLayout(
        {{ShaderDataType::Float3, "a_WorldPosition"},
         {ShaderDataType::Float3, "a_LocalPosition"},
         {ShaderDataType::Float4, "a_Color"},
         {ShaderDataType::Float, "a_Thickness"},
         {ShaderDataType::Float, "a_Fade"},
         {ShaderDataType::Int, "a_EntityID"}});
    s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);
    Ref<IndexBuffer> &quadIdxBuffer = const_cast<Ref<IndexBuffer> &>(
        s_Data.QuadVertexArray->GetIndexBuffer());

    /*
		Circle 도 왜 같은 Index buffer 를 활용할 수 있는 거지 ? 
		- 왜냐하면 지금 우리는 Circle 을 Quad 와 마찬가지로
		  4개의 정점을 활용하여 Circle 을 그려내고 있기 때문이다.

		  즉, 사실상 우리는 4각형 박스 범위 안에 Circle 을 그리고 있는 것이다.
		*/
    s_Data.CircleVertexArray->SetIndexBuffer(quadIdxBuffer); // Use quad IB
    s_Data.CircleVertexBufferBase = new CircleVertex[s_Data.MaxVertices];
}

void Renderer2D::initQuadVertexInfo()
{

    /*Square*/
    s_Data.QuadVertexArray = VertexArray::Create();

    // 5 floats per each vertex
    /*Vertex Pos + Texture Cordinate*/

    float squareVertices[5 * 4] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, /*Bottom Left  */
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, /*Bottom Right*/
        0.5f,  0.5f,  0.0f, 1.0f, 1.0f, /*Top Right*/
        -0.5f, 0.5f,  0.0f, 0.0f, 1.0f  /*Top Left*/
    };

    // Ref<VertexBuffer> squareVB;
    // squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));

    s_Data.QuadVertexBuffer =
        VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));

    BufferLayout squareVBLayout = {{ShaderDataType::Float3, "a_Position"},
                                   {ShaderDataType::Float4, "a_Color"},
                                   {ShaderDataType::Float2, "a_TexCoord"},
                                   {ShaderDataType::Float, "a_TexIndex"},
                                   {ShaderDataType::Float, "a_TilingFactor"},
                                   {ShaderDataType::Int, "a_EntityID"}};

    // squareVB->SetLayout(squareVBLayout);
    // s_Data.QuadVertexArray->AddVertexBuffer(squareVB);
    s_Data.QuadVertexBuffer->SetLayout(squareVBLayout);
    s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

    // 모든 Vertex 를 담을 수 있는 충분한 크기만큼 메모리를 할당한다.
    s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

    // uint32_t squareIndices[] = { 0, 1, 2, 2, 3, 0 };
    // Ref<IndexBuffer> squareIdxB;
    // squareIdxB = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
    // s_Data.QuadVertexArray->SetIndexBuffer(squareIdxB);
    uint32_t *quadIndices = new uint32_t[s_Data.MaxIndices];

    uint32_t offset = 0;

    /*
		ex) s_Data.MaxIndices 가 6000 개라고 한다면
			  하나의 Square 를 그리기 위한 index의 개수는 6개이다.

			  그러면 해당 index buffer 에 들어갈 데이터 크기는
			  index 하나의 크기 * 6개 씩 * 사각형 개수가 되는 것이다.
		*/
    for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
    {
        quadIndices[i + 0] = offset + 0;
        quadIndices[i + 1] = offset + 1;
        quadIndices[i + 2] = offset + 2;

        quadIndices[i + 3] = offset + 2;
        quadIndices[i + 4] = offset + 3;
        quadIndices[i + 5] = offset + 0;

        offset += 4;
    }
    Ref<IndexBuffer> quadIdxB =
        IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
    s_Data.QuadVertexArray->SetIndexBuffer(quadIdxB);
    delete[] quadIndices;

    // 일종의 기본 mesh (local pos)
    /*왼아 -> 오아 -> 오위 -> 왼위*/
    s_Data.QuadVertexPositions[0] = {-0.5f, -0.5f, 0.f, 1.f};
    s_Data.QuadVertexPositions[1] = {0.5f, -0.5f, 0.f, 1.f};
    s_Data.QuadVertexPositions[2] = {0.5f, 0.5f, 0.f, 1.f};
    s_Data.QuadVertexPositions[3] = {-0.5f, 0.5f, 0.f, 1.f};
}

void Renderer2D::initLineVertexInfo()
{
    // Lines
    s_Data.LineVertexArray = VertexArray::Create();

    s_Data.LineVertexBuffer =
        VertexBuffer::Create(s_Data.MaxVertices * sizeof(LineVertex));
    s_Data.LineVertexBuffer->SetLayout({{ShaderDataType::Float3, "a_Position"},
                                        {ShaderDataType::Float4, "a_Color"},
                                        {ShaderDataType::Int, "a_EntityID"}});
    s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);
    s_Data.LineVertexBufferBase = new LineVertex[s_Data.MaxVertices];

    /*
		기본적으로 Line 은 index buffer 를 사용하지 않을 것이다
		예를 들어
		 ㅡ
		|  |
		 ㅡ

		 이렇게 Line 4개를 그어서 Rect 를 만들 때
		 4개 정점을 duplicate 하여 사용할 것이다.

		 즉, 일반적으로 Quad 를 그릴 때는 4개의 정점과 6개의 idx buffer 정보를
		 이용해서 그린다. 4개의 정점을 6번 사용하지 않도록 하기 위함이다.

		 하지만 Line 은 그저 indx buffer 없이 , 그리는 방식으로 진행할
		 것이라는 의미이다.
		*/
}

void Renderer2D::initShaders()
{
    // Shader
    static std::string resourceRootPath = RESOURCE_ROOT;
    s_Data.QuadShader = Shader::Create(resourceRootPath +
                                       "assets/shaders/Renderer2D_Quad.glsl");
    s_Data.CircleShader = Shader::Create(
        resourceRootPath + "assets/shaders/Renderer2D_Circle.glsl");
    s_Data.LineShader = Shader::Create(resourceRootPath +
                                       "assets/shaders/Renderer2D_Line.glsl");
}

void Renderer2D::initTextures()
{

    s_Data.WhiteTexture = TextureManager::CreateTexture2D(1, 1);
    uint32_t whiteTextureData = 0xffffffff;
    s_Data.WhiteTexture->SetData(&whiteTextureData,
                                 /*1 * 1 */ sizeof(uint32_t));


    // s_Data.TextureShader->Bind();
    // s_Data.TextureShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);

    int samplers[s_Data.MaxTextureSlots];
    for (uint32_t i = 0; i < s_Data.MaxTextureSlots; ++i)
    {
        samplers[i] = i;
    }

#if OLD_PATH
    s_Data.TextureShader->SetInt("u_Texture", 0);
#endif

    for (uint32_t i = 0; i < s_Data.TextureSlots.size(); ++i)
    {
        s_Data.TextureSlots[i] = 0;
    }

    // bind default texture
    s_Data.TextureSlots[0] = s_Data.WhiteTexture;
}

void Renderer2D::initUniforms()
{
    s_Data.CameraUniformBuffer =
        UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
}

void Renderer2D::startBatch()
{
    s_Data.QuadIndexCount = 0;
    s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

    // Quad 와 Circle 을 분리해야 할 것 같다
    s_Data.CircleIndexCount = 0;
    s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

    s_Data.LineVertexCount = 0;
    s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

    s_Data.TextureSlotIndex = 1;
}

void Renderer2D::nextBatch()
{
    Flush();
    startBatch();
}


void Renderer2D::Flush()
{
    HZ_PROFILE_FUNCTION();

    for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
    {
        s_Data.TextureSlots[i]->Bind(i);
    }

    if (s_Data.QuadIndexCount)
    {
        // EndScene 에서 s_Data.QuadVertexBufferPtr 을 이용하여 쌓아놓은 정점 정보들을
        // 이용하여 한번에 그려낼 것이다.
        // - 포인터를 숫자 형태로 형변환하기 위해  (uint8_t*) 로 캐스팅한다.
        uint32_t dataSize = (uint32_t)((uint8_t *)s_Data.QuadVertexBufferPtr -
                                       (uint8_t *)s_Data.QuadVertexBufferBase);
        s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

        // Bind textures (해당 함수는.. 여기가 아니라 맨 위에서 해줘야 하는거 아닌가 ?)
        // 모든 Texture 를 한꺼번에 Bind 해야 한다.
        // 0 번째에 기본적으로 Binding 된 WhiteTexture 도 Bind 해줘야 한다.
        // for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
        // 	s_Data.TextureSlots[i]->Bind(i);

        s_Data.QuadShader->Bind();
        RenderCommand::DrawIndexed(s_Data.QuadVertexArray,
                                   s_Data.QuadIndexCount);
#if STATISTICS
        s_Data.stats.DrawCalls++;
#endif
    }
    if (s_Data.CircleIndexCount)
    {
        uint32_t dataSize =
            (uint32_t)((uint8_t *)s_Data.CircleVertexBufferPtr -
                       (uint8_t *)s_Data.CircleVertexBufferBase);
        s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferBase,
                                           dataSize);

        s_Data.CircleShader->Bind();
        RenderCommand::DrawIndexed(s_Data.CircleVertexArray,
                                   s_Data.CircleIndexCount);

#if STATISTICS
        s_Data.stats.DrawCalls++;
#endif
    }

    if (s_Data.LineVertexCount)
    {
        uint32_t dataSize = (uint32_t)((uint8_t *)s_Data.LineVertexBufferPtr -
                                       (uint8_t *)s_Data.LineVertexBufferBase);
        s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, dataSize);

        s_Data.LineShader->Bind();
        RenderCommand::SetLineWidth(s_Data.LineWidth);
        RenderCommand::DrawLines(s_Data.LineVertexArray,
                                 s_Data.LineVertexCount);

#if STATISTICS
        s_Data.stats.DrawCalls++;
#endif
    }
}

void Renderer2D::DrawQuad(const glm::vec2 &pos,
                          const glm::vec2 &size,
                          const glm::vec4 &color)
{
    DrawQuad({pos.x, pos.y, 0.f}, size, color);
}

void Renderer2D::DrawQuad(const glm::vec3 &pos,
                          const glm::vec2 &size,
                          const glm::vec4 &color)
{
    HZ_PROFILE_FUNCTION();

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) *
                          glm::scale(glm::mat4(1.f), {size.x, size.y, 1.f});

    DrawQuad(transform, color);
}

void Renderer2D::DrawQuad(const glm::vec2 &pos,
                          const glm::vec2 &size,
                          const Ref<Texture2D> &texture,
                          float tilingFactor,
                          const glm::vec4 &tintColor)
{
    HZ_PROFILE_FUNCTION();

    DrawQuad({pos.x, pos.y, 0.f}, size, texture, tilingFactor);
}

void Renderer2D::DrawQuad(const glm::vec3 &pos,
                          const glm::vec2 &size,
                          const Ref<Texture2D> &texture,
                          float tilingFactor,
                          const glm::vec4 &tintColor)
{
    HZ_PROFILE_FUNCTION();

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) *
                          glm::scale(glm::mat4(1.f), {size.x, size.y, 1.f});

    DrawQuad(transform, texture, tilingFactor, tintColor);

#if OLD_PATH
    s_Data.TextureShader->Bind();

    // default : 0번째 slot 에 세팅
    texture->Bind();

    // 기본 Color 로 세팅
    s_Data.TextureShader->SetFloat4("u_Color", tintColor);
    s_Data.TextureShader->SetFloat("m_TilingFactor", tilingFactor);

    // x,y 축 기준으로만 scale 을 조정할 것이다.
    glm::mat4 scale = glm::scale(glm::mat4(1.f), {size.x, size.y, 1.0f});
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) *
                          /*rotation*/ scale;

    s_Data.TextureShader->SetMat4("u_Transform", transform);

    // actual draw call
    s_Data.QuadVertexArray->Bind();

    // 해당 함수안에 Texture Bind 가 존재한다.
    RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
#endif
}

void Renderer2D::DrawQuad(const glm::vec2 &pos,
                          const glm::vec2 &size,
                          const Ref<SubTexture2D> &subTexture,
                          float tilingFactor,
                          const glm::vec4 &tintColor)
{
    HZ_PROFILE_FUNCTION();

    DrawQuad({pos.x, pos.y, 0.f}, size, subTexture, tilingFactor);
}

void Renderer2D::DrawQuad(const glm::vec3 &pos,
                          const glm::vec2 &size,
                          const Ref<SubTexture2D> &subTexture,
                          float tilingFactor,
                          const glm::vec4 &tintColor)
{
    HZ_PROFILE_FUNCTION();

    if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
    {
        // FlushAndReset();
        nextBatch();
    }

    constexpr glm::vec4 color = {1.f, 1.f, 1.f, 1.f};
    const glm::vec2 *textureCoords = subTexture->GetTexCoords();
    const Ref<Texture2D> texture = subTexture->GetTexture();

    // 현재 인자로 들어온 Texture 에 대한 s_Data.TextureSlot 내 Texture Index 를 찾아야 한다.
    float textureIndex = 0.f;

    for (uint32_t i = 0; i < s_Data.TextureSlotIndex; ++i)
    {
        // Texture2D& compTexture = *s_Data.TextureSlots[i].get();

        if (*s_Data.TextureSlots[i].get() == *texture.get())
        {
            textureIndex = (float)i;
            break;
        }
    }

    // new texture
    if (textureIndex == 0.f)
    {
        textureIndex = (float)s_Data.TextureSlotIndex;
        s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
        s_Data.TextureSlotIndex += 1;
    }

    // 시계 방향으로 4개의 정점 정보를 모두 세팅한다.
    // 왼쪽 아래
    s_Data.QuadVertexBufferPtr->Position = pos;
    s_Data.QuadVertexBufferPtr->Color = color;
    s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[0];
    s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
    s_Data.QuadVertexBufferPtr++;

    // 오른쪽 아래
    s_Data.QuadVertexBufferPtr->Position = {pos.x + size.x, pos.y, 0.f};
    s_Data.QuadVertexBufferPtr->Color = color;
    s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[1];
    s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
    s_Data.QuadVertexBufferPtr++;

    // 오른쪽 위
    s_Data.QuadVertexBufferPtr->Position = {pos.x + size.x,
                                            pos.y + size.y,
                                            0.f};
    s_Data.QuadVertexBufferPtr->Color = color;
    s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[2];
    s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
    s_Data.QuadVertexBufferPtr++;

    // 왼쪽 위
    s_Data.QuadVertexBufferPtr->Position = {pos.x, pos.y + size.y, 0.f};
    s_Data.QuadVertexBufferPtr->Color = color;
    s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[3];
    s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadIndexCount += 6;

#if STATISTICS
    s_Data.stats.QuadCount++;
#endif

#if OLD_PATH
    s_Data.TextureShader->Bind();

    // default : 0번째 slot 에 세팅
    texture->Bind();

    // 기본 Color 로 세팅
    s_Data.TextureShader->SetFloat4("u_Color", tintColor);
    s_Data.TextureShader->SetFloat("m_TilingFactor", tilingFactor);

    // x,y 축 기준으로만 scale 을 조정할 것이다.
    glm::mat4 scale = glm::scale(glm::mat4(1.f), {size.x, size.y, 1.0f});
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) *
                          /*rotation*/ scale;

    s_Data.TextureShader->SetMat4("u_Transform", transform);

    // actual draw call
    s_Data.QuadVertexArray->Bind();

    // 해당 함수안에 Texture Bind 가 존재한다.
    RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
#endif
}

void Renderer2D::DrawQuad(const glm::mat4 &transform,
                          const glm::vec4 &color,
                          int entityID)
{
    HZ_PROFILE_FUNCTION();

    if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
    {
        // FlushAndReset();
        nextBatch();
    }

    constexpr glm::vec2 textureCoords[] = {{0.f, 0.f},
                                           {1.f, 0.f},
                                           {1.f, 1.f},
                                           {0.f, 1.f}};

    const float texIndex = 0.f; // white texture
    const float tilingFactor = 1.f;

    // 시계 방향으로 4개의 정점 정보를 모두 세팅한다.
    // 왼쪽 아래
    // 오른쪽 아래
    // 오른쪽 위
    // 왼쪽 위

    constexpr size_t quadVertexCnt = 4;
    for (size_t i = 0; i < quadVertexCnt; ++i)
    {
        s_Data.QuadVertexBufferPtr->Position =
            transform * s_Data.QuadVertexPositions[i];
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
        s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
        s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr->EntityID = entityID;
        s_Data.QuadVertexBufferPtr++;
    }


    s_Data.QuadIndexCount += 6;

#if STATISTICS
    s_Data.stats.QuadCount++;
#endif
};

void Renderer2D::DrawQuad(const glm::mat4 &transform,
                          const Ref<Texture2D> &texture,
                          float tilingFactor,
                          const glm::vec4 &tintColor,
                          int entityID)
{
    HZ_PROFILE_FUNCTION();

    if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
    {
        // FlushAndReset();
        nextBatch();
    }

    constexpr glm::vec4 color = {1.f, 1.f, 1.f, 1.f};
    constexpr glm::vec2 textureCoords[] = {{0.f, 0.f},
                                           {1.f, 0.f},
                                           {1.f, 1.f},
                                           {0.f, 1.f}};

    // 현재 인자로 들어온 Texture 에 대한 s_Data.TextureSlot 내 Texture Index 를 찾아야 한다.
    float textureIndex = 0.f;

    for (uint32_t i = 0; i < s_Data.TextureSlotIndex; ++i)
    {
        if (*s_Data.TextureSlots[i].get() == *texture.get())
        {
            textureIndex = (float)i;
            break;
        }
    }

    // new texture
    if (textureIndex == 0.f)
    {
        textureIndex = (float)s_Data.TextureSlotIndex;
        s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
        s_Data.TextureSlotIndex += 1;
    }

    // 시계 방향으로 4개의 정점 정보를 모두 세팅한다.
    // 왼쪽 아래
    // 오른쪽 아래
    // 오른쪽 위
    // 왼쪽 위
    constexpr size_t quadVertexCnt = 4;

    for (size_t i = 0; i < quadVertexCnt; ++i)
    {
        s_Data.QuadVertexBufferPtr->Position =
            transform * s_Data.QuadVertexPositions[i];
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
        s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr->EntityID = entityID;
        s_Data.QuadVertexBufferPtr++;
    }

    s_Data.QuadIndexCount += 6;

#if STATISTICS
    s_Data.stats.QuadCount++;
#endif
}

void Renderer2D::DrawRotatedQuad(const glm::vec2 &pos,
                                 const glm::vec2 &size,
                                 float rotation,
                                 const glm::vec4 &color)
{
    HZ_PROFILE_FUNCTION();

    DrawRotatedQuad({pos.x, pos.y, 0.f}, size, rotation, color);
}
void Renderer2D::DrawRotatedQuad(const glm::vec3 &pos,
                                 const glm::vec2 &size,
                                 float rotation,
                                 const glm::vec4 &color)
{
    HZ_PROFILE_FUNCTION();

    if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
    {
        // FlushAndReset();
        nextBatch();
    }
    constexpr glm::vec2 textureCoords[] = {{0.f, 0.f},
                                           {1.f, 0.f},
                                           {1.f, 1.f},
                                           {0.f, 1.f}};

    glm::mat4 transform =
        glm::translate(glm::mat4(1.0f), pos) *
        glm::rotate(glm::mat4(1.f), rotation, {0.f, 0.f, 1.f}) // z 축 회전
        * glm::scale(glm::mat4(1.f), {size.x, size.y, 1.f});

    const float texIndex = 0.f; // white texture
    const float tilingFactor = 1.f;

    // 시계 방향으로 4개의 정점 정보를 모두 세팅한다.
    // 왼쪽 아래
    // 오른쪽 아래
    // 오른쪽 위
    // 왼쪽 위

    constexpr size_t quadVertexCnt = 4;
    for (size_t i = 0; i < quadVertexCnt; ++i)
    {
        s_Data.QuadVertexBufferPtr->Position =
            transform * s_Data.QuadVertexPositions[i];
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
        s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
        s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr++;
    }

    s_Data.QuadIndexCount += 6;

#if STATISTICS
    s_Data.stats.QuadCount++;
#endif

#if one
    // 혹시나 문제 생기면, 여기에 Shader 한번 더 bind
    s_Data.TextureShader->SetFloat4("u_Color", color);
    s_Data.TextureShader->SetFloat("m_TilingFactor", 1.0f);

    // Bind Default White Texture
    s_Data.WhiteTexture->Bind();

    // x,y 축 기준으로만 scale 을 조정할 것이다.
    glm::mat4 scale = glm::scale(glm::mat4(1.f), {size.x, size.y, 1.0f});

    glm::mat4 transform =
        glm::translate(glm::mat4(1.0f), pos)
        // 2D renderer 이므로 z 축 회전을 적용한다.
        * glm::rotate(glm::mat4(1.f), rotation, {0.f, 0.f, 1.f}) * scale;

    s_Data.TextureShader->SetMat4("u_Transform", transform);

    // actual draw call
    s_Data.QuadVertexArray->Bind();
    RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
#endif
}
void Renderer2D::DrawRotatedQuad(const glm::vec2 &pos,
                                 const glm::vec2 &size,
                                 float rotation,
                                 const Ref<Texture2D> &texture,
                                 float tilingFactor,
                                 const glm::vec4 &tintColor)
{
    HZ_PROFILE_FUNCTION();

    DrawRotatedQuad({pos.x, pos.y, 0.f},
                    size,
                    rotation,
                    texture,
                    tilingFactor,
                    tintColor);
}
void Renderer2D::DrawRotatedQuad(const glm::vec3 &pos,
                                 const glm::vec2 &size,
                                 float rotation,
                                 const Ref<Texture2D> &texture,
                                 float tilingFactor,
                                 const glm::vec4 &tintColor)
{
    HZ_PROFILE_FUNCTION();

    constexpr glm::vec4 color = {1.f, 1.f, 1.f, 1.f};
    constexpr glm::vec2 textureCoords[] = {{0.f, 0.f},
                                           {1.f, 0.f},
                                           {1.f, 1.f},
                                           {0.f, 1.f}};

    // 현재 인자로 들어온 Texture 에 대한 s_Data.TextureSlot 내 Texture Index 를 찾아야 한다.
    float textureIndex = 0.f;

    for (uint32_t i = 0; i < s_Data.TextureSlotIndex; ++i)
    {
        // Texture2D& compTexture = *s_Data.TextureSlots[i].get();

        if (*s_Data.TextureSlots[i].get() == *texture.get())
        {
            textureIndex = (float)i;
            break;
        }
    }

    // new texture
    if (textureIndex == 0.f)
    {
        textureIndex = (float)s_Data.TextureSlotIndex;
        s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
        s_Data.TextureSlotIndex += 1;
    }

    glm::mat4 transform =
        glm::translate(glm::mat4(1.0f), pos) *
        glm::rotate(glm::mat4(1.f), rotation, {0.f, 0.f, 1.f}) // z 축 회전
        * glm::scale(glm::mat4(1.f), {size.x, size.y, 1.f});

    // 시계 방향으로 4개의 정점 정보를 모두 세팅한다.
    // 왼쪽 아래
    // 오른쪽 아래
    // 오른쪽 위
    // 왼쪽 위
    // s_Data.QuadVertexBufferPtr->Position = pos;
    // transform * s_Data.QuadVertexPositions[0] : opengl 오른손 좌표계 반영 위해, mat4 를 vec4 앞에 곱한다.
    constexpr size_t quadVertexCnt = 4;

    for (size_t i = 0; i < quadVertexCnt; ++i)
    {
        s_Data.QuadVertexBufferPtr->Position =
            transform * s_Data.QuadVertexPositions[i];
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
        s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr++;
    }

    s_Data.QuadIndexCount += 6;

#if STATISTICS
    s_Data.stats.QuadCount++;
#endif

#if one
    // 혹시나 문제 생기면, 여기에 Shader 한번 더 bind
    s_Data.TextureShader->SetFloat4("u_Color", tintColor);
    s_Data.TextureShader->SetFloat("m_TilingFactor", tilingFactor);

    // Bind Default White Texture
    s_Data.WhiteTexture->Bind();

    // x,y 축 기준으로만 scale 을 조정할 것이다.
    glm::mat4 scale = glm::scale(glm::mat4(1.f), {size.x, size.y, 1.0f});

    glm::mat4 transform =
        glm::translate(glm::mat4(1.0f), pos)
        // 2D renderer 이므로 z 축 회전을 적용한다.
        * glm::rotate(glm::mat4(1.f), rotation, {0.f, 0.f, 1.f}) * scale;

    s_Data.TextureShader->SetMat4("u_Transform", transform);

    // actual draw call
    s_Data.QuadVertexArray->Bind();

    RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
#endif
}


void Renderer2D::DrawRotatedQuad(const glm::vec2 &pos,
                                 const glm::vec2 &size,
                                 float rotation,
                                 const Ref<SubTexture2D> &subTexture,
                                 float tilingFactor,
                                 const glm::vec4 &tintColor)
{
    HZ_PROFILE_FUNCTION();

    DrawRotatedQuad({pos.x, pos.y, 0.f},
                    size,
                    rotation,
                    subTexture,
                    tilingFactor,
                    tintColor);
}
void Renderer2D::DrawRotatedQuad(const glm::vec3 &pos,
                                 const glm::vec2 &size,
                                 float rotation,
                                 const Ref<SubTexture2D> &subTexture,
                                 float tilingFactor,
                                 const glm::vec4 &tintColor)
{
    HZ_PROFILE_FUNCTION();

    constexpr glm::vec4 color = {1.f, 1.f, 1.f, 1.f};

    const glm::vec2 *textureCoords = subTexture->GetTexCoords();
    const Ref<Texture2D> texture = subTexture->GetTexture();

    // 현재 인자로 들어온 Texture 에 대한 s_Data.TextureSlot 내 Texture Index 를 찾아야 한다.
    float textureIndex = 0.f;

    for (uint32_t i = 0; i < s_Data.TextureSlotIndex; ++i)
    {
        // Texture2D& compTexture = *s_Data.TextureSlots[i].get();

        if (*s_Data.TextureSlots[i].get() == *texture.get())
        {
            textureIndex = (float)i;
            break;
        }
    }

    // new texture
    if (textureIndex == 0.f)
    {
        textureIndex = (float)s_Data.TextureSlotIndex;
        s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
        s_Data.TextureSlotIndex += 1;
    }

    glm::mat4 transform =
        glm::translate(glm::mat4(1.0f), pos) *
        glm::rotate(glm::mat4(1.f), rotation, {0.f, 0.f, 1.f}) // z 축 회전
        * glm::scale(glm::mat4(1.f), {size.x, size.y, 1.f});

    constexpr size_t quadVertexCnt = 4;

    // 시계 방향으로 4개의 정점 정보를 모두 세팅한다.
    // 왼쪽 아래
    // s_Data.QuadVertexBufferPtr->Position = pos;
    // transform * s_Data.QuadVertexPositions[0] : opengl 오른손 좌표계 반영 위해, mat4 를 vec4 앞에 곱한다.
    for (size_t i = 0; i < quadVertexCnt; ++i)
    {
        s_Data.QuadVertexBufferPtr->Position =
            transform * s_Data.QuadVertexPositions[i];
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
        s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr++;
    }

    s_Data.QuadIndexCount += 6;

#if STATISTICS
    s_Data.stats.QuadCount++;
#endif

#if one
    // 혹시나 문제 생기면, 여기에 Shader 한번 더 bind
    s_Data.TextureShader->SetFloat4("u_Color", tintColor);
    s_Data.TextureShader->SetFloat("m_TilingFactor", tilingFactor);

    // Bind Default White Texture
    s_Data.WhiteTexture->Bind();

    // x,y 축 기준으로만 scale 을 조정할 것이다.
    glm::mat4 scale = glm::scale(glm::mat4(1.f), {size.x, size.y, 1.0f});

    glm::mat4 transform =
        glm::translate(glm::mat4(1.0f), pos)
        // 2D renderer 이므로 z 축 회전을 적용한다.
        * glm::rotate(glm::mat4(1.f), rotation, {0.f, 0.f, 1.f}) * scale;

    s_Data.TextureShader->SetMat4("u_Transform", transform);

    // actual draw call
    s_Data.QuadVertexArray->Bind();

    RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
#endif
}

void Renderer2D::DrawLine(const glm::vec3 &p0,
                          glm::vec3 &p1,
                          const glm::vec4 &color,
                          int entityID)
{
    s_Data.LineVertexBufferPtr->Position = p0;
    s_Data.LineVertexBufferPtr->Color = color;
    s_Data.LineVertexBufferPtr->EntityID = entityID;
    s_Data.LineVertexBufferPtr++;

    s_Data.LineVertexBufferPtr->Position = p1;
    s_Data.LineVertexBufferPtr->Color = color;
    s_Data.LineVertexBufferPtr->EntityID = entityID;
    s_Data.LineVertexBufferPtr++;

    s_Data.LineVertexCount += 2;
}

void Renderer2D::DrawRect(const glm::vec3 &position,
                          const glm::vec2 &size,
                          const glm::vec4 &color,
                          int entityID)
{
    glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f,
                             position.y - size.y * 0.5f,
                             position.z);
    glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f,
                             position.y - size.y * 0.5f,
                             position.z);
    glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f,
                             position.y + size.y * 0.5f,
                             position.z);
    glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f,
                             position.y + size.y * 0.5f,
                             position.z);

    DrawLine(p0, p1, color, entityID);
    DrawLine(p1, p2, color, entityID);
    DrawLine(p2, p3, color, entityID);
    DrawLine(p3, p0, color, entityID);
}

void Renderer2D::DrawRect(const glm::mat4 &transform,
                          const glm::vec4 &color,
                          int entityID)
{
    glm::vec3 lineVertices[4];
    for (size_t i = 0; i < 4; i++)
        lineVertices[i] = transform * s_Data.QuadVertexPositions[i];

    DrawLine(lineVertices[0], lineVertices[1], color, entityID);
    DrawLine(lineVertices[1], lineVertices[2], color, entityID);
    DrawLine(lineVertices[2], lineVertices[3], color, entityID);
    DrawLine(lineVertices[3], lineVertices[0], color, entityID);
}

void Renderer2D::DrawCircle(const glm::mat4 &transform,
                            const glm::vec4 &color,
                            float thickness,
                            float fade,
                            int entityID)
{
    HZ_PROFILE_FUNCTION();

    // TODO: implement for circles
    // if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
    // 	NextBatch();

    for (size_t i = 0; i < 4; i++)
    {
        s_Data.CircleVertexBufferPtr->WorldPosition =
            transform * s_Data.QuadVertexPositions[i];
        /*x,y 가 -1 에서 1 사이의 값을 가지게 된다.*/
        s_Data.CircleVertexBufferPtr->LocalPosition =
            s_Data.QuadVertexPositions[i] * 2.0f;
        s_Data.CircleVertexBufferPtr->Color = color;
        s_Data.CircleVertexBufferPtr->Thickness = thickness;
        s_Data.CircleVertexBufferPtr->Fade = fade;
        s_Data.CircleVertexBufferPtr->EntityID = entityID;
        s_Data.CircleVertexBufferPtr++;
    }

    s_Data.CircleIndexCount += 6;

    s_Data.stats.QuadCount++;
}

void Renderer2D::DrawSprite(const glm::mat4 &transform,
                            SpriteRenderComponent &src,
                            int entityID)
{
    if (src.GetTexture())
        DrawQuad(transform,
                 src.GetTexture(),
                 src.GetTilingFactor(),
                 src.GetColor(),
                 entityID);
    else
        DrawQuad(transform, src.GetColor(), entityID);
}


float Renderer2D::GetLineWidth()
{
    return s_Data.LineWidth;
}

void Renderer2D::SetLineWidth(float width)
{
    s_Data.LineWidth = width;
}

Renderer2D::Statistics Renderer2D::GetStats()
{
    return s_Data.stats;
}
void Renderer2D::ResetStats()
{
    memset(&s_Data.stats, 0, sizeof(Renderer2D::Statistics));
}
} // namespace Hazel
