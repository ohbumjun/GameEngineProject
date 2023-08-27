#include "hzpch.h"
#include "Renderer2D.h"
#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"
#include <glm/gtc/matrix_transform.hpp>

#define STATISTICS 1

namespace Hazel
{
	// 각 정점이 가지고 있어야할 정보
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;			 // Texture Slot 상 mapping 된 index
		float TilingFactor;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads    = 10000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices  = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; 

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		// 총 몇개의 quad indice 가 그려지고 있는가
		// Quad 를 그릴 때마다 + 6 해줄 것이다.
		uint32_t QuadIndexCount = 0;

		// QuadVertex 들을 담은 배열.을 가리키는 포인터
		QuadVertex* QuadVertexBufferBase = nullptr;

		// QuadVertexBufferBase 라는 배열 내에서 각 원소를 순회하기 위한 포인터
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 : Default White Texture 로 세팅

		// mesh local pos
		glm::vec4 QuadVertexPositions[4];

		Renderer2D::Statistics stats;
	};

	static Renderer2DData s_Data;

	void Renderer2D::Init()
	{
		HZ_PROFILE_FUNCTION();

		/*Square*/
		s_Data.QuadVertexArray = VertexArray::Create();

		// 5 floats per each vertex
		/*Vertex Pos + Texture Cordinate*/

		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  /*Bottom Left  */
			0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  /*Bottom Right*/
			0.5f,  0.5f, 0.0f, 1.0f, 1.0f,   /*Top Right*/
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f    /*Top Left*/
		};

		// Ref<VertexBuffer> squareVB;
		// squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		
		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));

		BufferLayout squareVBLayout = {
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"},
			{ShaderDataType::Float2, "a_TexCoord"},
			{ShaderDataType::Float,   "a_TexIndex"},
			{ShaderDataType::Float,   "a_TilingFactor"}
		};

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
		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];

		uint32_t offset = 0;

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
		Ref<IndexBuffer> quadIdxB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIdxB);
		delete [] quadIndices;

		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, /*1 * 1 */sizeof(uint32_t));

		s_Data.TextureShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data.TextureShader->Bind();

		int samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; ++i)
		{
			samplers[i] = i;
		}
		s_Data.TextureShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);

#if OLD_PATH
		s_Data.TextureShader->SetInt("u_Texture", 0);
#endif

		for (uint32_t i = 0; i < s_Data.TextureSlots.size(); ++i)
		{
			s_Data.TextureSlots[i] = 0;
		}

		// bind default texture
		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		// 일종의 기본 mesh (local pos)
		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.f, 1.f };
		s_Data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.f, 1.f };
		s_Data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.f, 1.f };
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.f, 1.f };
	}

	void Renderer2D::ShutDown()
	{
		HZ_PROFILE_FUNCTION();

		delete s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		HZ_PROFILE_FUNCTION();

		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4(
			"u_ViewProjection", const_cast<OrthographicCamera&>(camera).GetViewProjectionMatrix());
	
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.QuadIndexCount = 0;

		// 0 
		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::EndScene()
	{
		HZ_PROFILE_FUNCTION();

		// EndScene 에서 s_Data.QuadVertexBufferPtr 을 이용하여 쌓아놓은 정점 정보들을
		// 이용하여 한번에 그려낼 것이다.
		// - 포인터를 숫자 형태로 형변환하기 위해  (uint8_t*) 로 캐스팅한다.
		uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

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


	void Renderer2D::Flush()
	{
		HZ_PROFILE_FUNCTION();

		// 모든 Texture 를 한꺼번에 Bind 해야 한다.
		// 0 번째에 기본적으로 Binding 된 WhiteTexture 도 Bind 해줘야 한다.
		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; ++i)
		{
			s_Data.TextureSlots[i]->Bind(i);
		}

		// Batch Rendering 의 경우, 한번의 DrawCall 을 한다.
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);

#if STATISTICS
		s_Data.stats.DrawCalls++;
#endif
	}

	void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ pos.x, pos.y, 0.f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			FlushAndReset();
		}

#if STATISTICS

#endif

		const float texIndex = 0.f; // white texture
		const float tilingFactor = 1.f;

		// 시계 방향으로 4개의 정점 정보를 모두 세팅한다.
		// 왼쪽 아래
		s_Data.QuadVertexBufferPtr->Position = pos;
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = {0.f, 0.f}; 
		s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		// 오른쪽 아래
		s_Data.QuadVertexBufferPtr->Position = { pos.x + size.x, pos.y, 0.f };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.f, 0.f };
		s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		// 오른쪽 위
		s_Data.QuadVertexBufferPtr->Position = { pos.x + size.x, pos.y + size.y, 0.f };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.f, 1.f };
		s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		// 왼쪽 위
		s_Data.QuadVertexBufferPtr->Position = { pos.x, pos.y + size.y, 0.f };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.f, 1.f };
		s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadIndexCount += 6;

#if STATISTICS
		s_Data.stats.QuadCount++;
#endif

		/*
		아래는 Batch Rendering 이 아니라, 하나의 단일 Quad 를 그릴 경우 코드
		Batch Rendering 에서는 EndScene 에서 한꺼번에 처리해줄 것이다.
		*/
#if OLD_PATH
		// 혹시나 문제 생기면, 여기에 Shader 한번 더 bind
		s_Data.TextureShader->SetFloat4("u_Color", color);
		s_Data.TextureShader->SetFloat("m_TilingFactor", 1.0f);

		// Bind Default White Texture
		s_Data.WhiteTexture->Bind();

		// x,y 축 기준으로만 scale 을 조정할 것이다.
		glm::mat4 scale = glm::scale(glm::mat4(1.f), {size.x, size.y, 1.0f});
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * 
			scale;

		s_Data.TextureShader->SetMat4("u_Transform", transform);

		// actual draw call
		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
#endif
	}

	void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, 
		const Ref<Texture2D>&texture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();

		DrawQuad({ pos.x, pos.y, 0.f }, size, texture, tilingFactor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, 
		const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			FlushAndReset();
		}

		constexpr glm::vec4 color = { 1.f, 1.f, 1.f, 1.f };

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
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.f, 0.f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		// 오른쪽 아래
		s_Data.QuadVertexBufferPtr->Position = { pos.x + size.x, pos.y, 0.f };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.f, 0.f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		// 오른쪽 위
		s_Data.QuadVertexBufferPtr->Position = { pos.x + size.x, pos.y + size.y, 0.f };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.f, 1.f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		// 왼쪽 위
		s_Data.QuadVertexBufferPtr->Position = { pos.x, pos.y + size.y, 0.f };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.f, 1.f };
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
		glm::mat4 scale = glm::scale(glm::mat4(1.f), { size.x, size.y, 1.0f });
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) *
			/*rotation*/ scale;

		s_Data.TextureShader->SetMat4("u_Transform", transform);

		// actual draw call
		s_Data.QuadVertexArray->Bind();

		// 해당 함수안에 Texture Bind 가 존재한다.
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
#endif
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();

		DrawRotatedQuad({ pos.x, pos.y, 0.f }, size, rotation, color);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			FlushAndReset();
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
			* glm::rotate(glm::mat4(1.f), rotation, { 0.f, 0.f, 1.f }) // z 축 회전
			* glm::scale(glm::mat4(1.f), { size.x, size.y, 1.f });

		const float texIndex = 0.f; // white texture
		const float tilingFactor = 1.f;

		// 시계 방향으로 4개의 정점 정보를 모두 세팅한다.
		// 왼쪽 아래
		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.f, 0.f };
		s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		// 오른쪽 아래
		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[1];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.f, 0.f };
		s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		// 오른쪽 위
		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[2];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.f, 1.f };
		s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		// 왼쪽 위
		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[3];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.f, 1.f };
		s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

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
		glm::mat4 scale = glm::scale(glm::mat4(1.f), { size.x, size.y, 1.0f });

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) 
			// 2D renderer 이므로 z 축 회전을 적용한다.
			* glm::rotate(glm::mat4(1.f), rotation, {0.f, 0.f, 1.f})
			* scale;

		s_Data.TextureShader->SetMat4("u_Transform", transform);

		// actual draw call
		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
#endif
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, 
		float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();

		DrawRotatedQuad({ pos.x, pos.y, 0.f }, size, rotation, texture, tilingFactor, tintColor);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size,
		float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();

		constexpr glm::vec4 color = { 1.f, 1.f, 1.f, 1.f };

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

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
			* glm::rotate(glm::mat4(1.f), rotation, { 0.f, 0.f, 1.f }) // z 축 회전
			* glm::scale(glm::mat4(1.f), { size.x, size.y, 1.f });

		// 시계 방향으로 4개의 정점 정보를 모두 세팅한다.
		// 왼쪽 아래
		// s_Data.QuadVertexBufferPtr->Position = pos;
		// transform * s_Data.QuadVertexPositions[0] : opengl 오른손 좌표계 반영 위해, mat4 를 vec4 앞에 곱한다.
		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.f, 0.f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		// 오른쪽 아래
		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[1];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.f, 0.f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		// 오른쪽 위
		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[2];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.f, 1.f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		// 왼쪽 위
		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[3];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.f, 1.f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

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
		glm::mat4 scale = glm::scale(glm::mat4(1.f), { size.x, size.y, 1.0f });

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
			// 2D renderer 이므로 z 축 회전을 적용한다.
			* glm::rotate(glm::mat4(1.f), rotation, { 0.f, 0.f, 1.f })
			* scale;

		s_Data.TextureShader->SetMat4("u_Transform", transform);

		// actual draw call
		s_Data.QuadVertexArray->Bind();

		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
#endif
	}
	
	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.stats;
	}
	void Renderer2D::ResetStats()
	{
		memset(&s_Data.stats, 0, sizeof(Renderer2D::Statistics));
	}
}

