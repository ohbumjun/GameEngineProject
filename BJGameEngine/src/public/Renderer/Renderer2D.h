#pragma once

#include "Camera/OrthographicCamera.h"
#include "Hazel/Asset/Image/Texture.h"
#include "Hazel/Asset/Image/SubTexture2D.h"
#include "Renderer/Camera/Camera.h"
#include "Renderer/Camera/EditorCamera.h"
#include "Hazel/Scene/Component/Renderer/SpriteRenderComponent.h"

namespace Hazel
{
	// 역할 : Scene 등 데이터를 받아서, 그에 따라 GPU 측에 그려달라고 요청하는 것
	// - 해당 Class 는 super static 이 될 것이다. 즉, 아무런 data storage 도
	// - 들고 있게 하지 않을 것이라는 의미이다.
class HAZEL_API Renderer2D
	{
	public:
		static void Init();
		static void ShutDown();
		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const OrthographicCamera& camera);
		static void BeginScene(const EditorCamera& camera);
		static void EndScene();

		/*
		더 발전시키려면 CircleFlush, QuadFlush 를
		구분하는 것이 좋다.
		*/
		static void Flush();

		// Primitives
		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size,
			const glm::vec4& color);
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size,
			const glm::vec4& color);
		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size,
			const Ref<Texture2D>& texture, float tilingFactor = 1.f,
			const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size,
			const Ref<Texture2D>& texture, float tilingFactor = 1.f,
			const glm::vec4& tintColor = glm::vec4(1.0f));

		// Draw With Atlas
		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size,
			const Ref<SubTexture2D>& subTexture, float tilingFactor = 1.f,
			const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size,
			const Ref<SubTexture2D>& subTexture, float tilingFactor = 1.f,
			const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, 
			float tilingFactor = 1.f, const glm::vec4& tintColor = glm::vec4(1.0f), int entityID = -1);

		/*
		* rotation 은 radian 형태로 client 에서 넘겨주게 할 것이다
		* 물론, 일반 각도를 받아서 engine 단에서 그것을 변형해줄 수도 있겠지만, 이는 성능 저하를 일으킬 것이다
		* Client 단에서 할 수 있는 사항들은 Client 가 처리할 수 있게 변경할 것이다.
		*/
		static void DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, float rotation,
			const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, float rotation,
			const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, float rotation,
			const Ref<Texture2D>& texture, float tilingFactor = 1.f,
			const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, float rotation,
			const Ref<Texture2D>& texture, float tilingFactor = 1.f,
			const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, float rotation,
			const Ref<SubTexture2D>& subTexture, float tilingFactor = 1.f,
			const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, float rotation,
			const Ref<SubTexture2D>& subTexture, float tilingFactor = 1.f,
			const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID = -1);

		static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
		static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);

		static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);

		static void DrawSprite(const glm::mat4& transform, SpriteRenderComponent& src, int entityID);

		// 매Update 초반 마다 reset 해줄 것이다.
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;

			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};

		static float GetLineWidth();
		static void SetLineWidth(float width);
		static Statistics GetStats();
		static void ResetStats();
		static void FlushAndReset();

	private :
		static void initCircleVertexInfo();
		static void initQuadVertexInfo();
		static void initLineVertexInfo();
		static void initShaders();
		static void initTextures();
		static void initUniforms();

		static void startBatch();
		static void nextBatch();
	};
};

