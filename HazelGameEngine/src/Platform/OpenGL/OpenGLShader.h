#pragma once

#include "Renderer/Shader.h"
#include <glm/glm.hpp>

namespace Hazel
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		void Bind() const override;
		void Unbind() const override;

		void UploadUniformBool(const std::string& name, const bool& val);

		void UploadUniformInt(const std::string& name, const int& val);
		// void UploadUniformInt2(const std::string& name, const int& mat);
		// void UploadUniformInt3(const std::string& name, const int& mat);
		// void UploadUniformInt4(const std::string& name, const int& mat);

		void UploadUniformFloat(const std::string& name, const float& val);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& val);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& val);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& val);

		void UploadUniformMat3(const std::string& name, const glm::mat3& mat);
		void UploadUniformMat4(const std::string& name, const glm::mat4& mat);

	private:
		// 현재 해당 객체를 식벽하는 ID
		uint32_t m_RendererID;
	};
}
