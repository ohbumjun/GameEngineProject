#include "hzpch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Hazel
{
	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		// Color Buffer 를 지울 때, 명확한 색상을 지정한다.
		// 해당 색상으로 아래의 Clear 함수에서 실제 지워준다.
		glClearColor(color.r, color.g, color.r, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		// Buffer 들을 미리 설정된 값으로 지운다.
		// Frame Buffer, Depth Buffer 도 지워주기 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::Init()
	{
		// Blending 허용
		glEnable(GL_BLEND);

		// Blending Function 정의 ex) glBlendFunc(src, dest)
		// - src   : src RGBA 계산 방법 (기본 : GL_ONE == 현재 그리는 색상 그대로 가져감)
		// - dest : dest RGBA 계산법  (기본 : GL_ZERO == 기존 그려진 색상 모두 지움)
		//            즉, 기본설정값 1 + 0 = 1

		// 아래 설정값 적용
		// ex 1) 현재 그리는 색상이 아예 투명하다면 
		// - src alpha = 0 / dest alpha = 1 - 0 = 1
		// ex 1) 현재 그리는 색상이 아예 '반' 투명하다면 
		// - src alpha = 0.5 / dest alpha = 1 - 0.5 = 0.5
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray,
		uint32_t indexCount)
	{
		uint32_t count = (indexCount == 0) ? vertexArray->GetIndexBuffer()->GetCount() : indexCount;
		
		glDrawElements(
			GL_TRIANGLES, 
			count,						// indices 개수 ex) 9 개 => GL_TRIANGLES : 3개씩 하나의 도형으로 인식하여 그리는 것. (삼각형)
											// 참고 : glDrawArrays는 다시 지나는 정점도 모두 데이터에 저장해준 반면, glDrawElements를 사용하면 한 정점은 한 번만 배열에 저장하여 사용할 수 있다.
			GL_UNSIGNED_INT,
			nullptr);

		// Open GL 에 Bind 된 Texture Unbind 시키기
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		// Window Resize 에 따라 ViewPort 재설정
		glViewport(x, y, width, height);
	}
}

