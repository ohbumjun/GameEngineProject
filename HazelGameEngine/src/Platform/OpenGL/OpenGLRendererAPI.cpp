#include "hzpch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Hazel
{
	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.r, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
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



	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(),
			GL_UNSIGNED_INT,
			nullptr);
	}

}

