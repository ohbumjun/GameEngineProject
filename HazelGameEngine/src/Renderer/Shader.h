#pragma once

#include <string>

namespace Hazel
{
	class Shader
	{
	public :
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();

		void Bind() const;
		void Unbind() const;

	private :
		// 현재 해당 객체를 식벽하는 ID
		uint32_t m_RendererID;
	};
}