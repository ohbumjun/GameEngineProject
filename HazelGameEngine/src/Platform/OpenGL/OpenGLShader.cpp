#include "hzpch.h"
#include "OpenGLShader.h"
#include "Renderer/Renderer.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Hazel
{
	OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc) 
	{
		// Create an empty vertex OpenGLShader handle (create OpenGLShader)
		// GLuint vertexOpenGLShader = glCreateOpenGLShader(GL_VERTEX_OpenGLShader);
		GLuint vertexOpenGLShader = glCreateShader(GL_VERTEX_SHADER);
		

		// Send the vertex OpenGLShader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = vertexSrc.c_str();
		glShaderSource(vertexOpenGLShader, 1, &source, 0);

		// Compile the vertex OpenGLShader
		glCompileShader(vertexOpenGLShader);

		// Check if compliation succeded or failed
		GLint isCompiled = 0;
		glGetShaderiv(vertexOpenGLShader, GL_COMPILE_STATUS, &isCompiled);

		// Fail Compile
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;

			// Get Length of logs (error message)
			glGetShaderiv(vertexOpenGLShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexOpenGLShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the OpenGLShader anymore.
			// (어차피 return 해서 나갈 것임)
			glDeleteShader(vertexOpenGLShader);

			HZ_CORE_ERROR("vertex OpenGLShader compliation failure");
			HZ_CORE_ERROR("{0}", infoLog.data());
			HZ_CORE_ASSERT(false, "!");
			return;
		}

		// Create an empty fragment OpenGLShader handle
		GLuint fragmentOpenGLShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment OpenGLShader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = fragmentSrc.c_str();
		glShaderSource(fragmentOpenGLShader, 1, &source, 0);

		// Compile the fragment OpenGLShader
		glCompileShader(fragmentOpenGLShader);

		glGetShaderiv(fragmentOpenGLShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentOpenGLShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentOpenGLShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the OpenGLShader anymore.
			glDeleteShader(fragmentOpenGLShader);

			// Either of them. Don't leak OpenGLShaders.
			glDeleteShader(vertexOpenGLShader);

			HZ_CORE_ERROR("fragment OpenGLShader compliation failure");
			HZ_CORE_ERROR("{0}", infoLog.data());
			HZ_CORE_ASSERT(false, "!");
			return;
		}

		// Vertex and fragment OpenGLShaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		// (참고 : OpenGLShader 라는 소스코드를 컴파일 하면 실행시킬 수 있는 프로그램을 얻는다)
		// GLuint program = glCreateProgram();
		m_RendererID = glCreateProgram();
		GLuint program = m_RendererID;

		// Attach our OpenGLShaders to our program
		glAttachShader(program, vertexOpenGLShader);
		glAttachShader(program, fragmentOpenGLShader);

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetOpenGLShader*.
		// Check Linking Failure
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak OpenGLShaders either.
			glDeleteShader(vertexOpenGLShader);
			glDeleteShader(fragmentOpenGLShader);
			HZ_CORE_ERROR("OpenGLShader link failure");
			HZ_CORE_ERROR("{0}", infoLog.data());
			HZ_CORE_ASSERT(false, "!");
			return;
		}

		// Always detach OpenGLShaders after a successful link.
		glDetachShader(program, vertexOpenGLShader);
		glDetachShader(program, fragmentOpenGLShader);
	}
	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}
	void OpenGLShader::Bind() const
	{
		// 해당 OpenGLShader 로 무언가를 그리고 싶을 때
		// 해당 함수를 먼저 실행할 것이다.
		glUseProgram(m_RendererID);
	}
	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}
	void OpenGLShader::UploadUniformBool(const std::string& name, const bool& val)
	{
	}
	void OpenGLShader::UploadUniformInt(const std::string& name, const int& val)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, val);
	}
	void OpenGLShader::UploadUniformFloat(const std::string& name, const float& val)
	{
		// OpenGLShader 에 선언된 uniform 변수를 가져오기
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());

		// uniform 변수를 OpenGLShader 쪽에 세팅해주는 함수
		// - 1 : 1개의 matrix 를 넘긴다
		glUniform1f(location, val);
	}
	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& vec)
	{
		// OpenGLShader 에 선언된 uniform 변수를 가져오기
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());

		// uniform 변수를 OpenGLShader 쪽에 세팅해주는 함수
		// - 1 : 1개의 matrix 를 넘긴다
		glUniform2f(location, vec.x, vec.y);
	}
	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& val)
	{
		// OpenGLShader 에 선언된 uniform 변수를 가져오기
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());

		// uniform 변수를 OpenGLShader 쪽에 세팅해주는 함수
		// - 1 : 1개의 matrix 를 넘긴다
		glUniform3f(location, val.x, val.y, val.z);
	}
	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& vec)
	{
		// OpenGLShader 에 선언된 uniform 변수를 가져오기
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());

		// uniform 변수를 OpenGLShader 쪽에 세팅해주는 함수
		// - 1 : 1개의 matrix 를 넘긴다
		glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
	}
	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& mat)
	{
		// OpenGLShader 에 선언된 uniform 변수를 가져오기
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());

		// uniform 변수를 OpenGLShader 쪽에 세팅해주는 함수
		// - 1 : 1개의 matrix 를 넘긴다
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat));
	}
	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& mat)
	{
		// OpenGLShader 에 선언된 uniform 변수를 가져오기
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());

		// uniform 변수를 OpenGLShader 쪽에 세팅해주는 함수
		// - 1 : 1개의 matrix 를 넘긴다
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
	}
}