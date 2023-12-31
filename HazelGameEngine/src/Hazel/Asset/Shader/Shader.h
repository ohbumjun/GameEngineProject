#pragma once

#include <glm/glm.hpp>

namespace Hazel
{
	class Shader
	{
	public :
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;

		virtual const std::string& GetName() const = 0;

		static Ref<Shader> Create(std::string_view path);
		static Ref<Shader> Create(std::string_view name, const std::string& vertexSrc, const std::string& fragmentSrc);
	};

	// Renderer Init 시에 필요한 Shader 파일들을 미리 컴파일 + Load 해두고/
	// User 가 필요할 때 해당 Shader 를 사용하게 하기
	class ShaderLibrary
	{
	public :
		void Add(const Ref<Shader>& shader);
		void Add(std::string_view name, const Ref<Shader>& shader);
		Ref<Shader> Load(std::string_view filePath);
		Ref<Shader> Load(std::string_view name, std::string_view filePath);

		Ref<Shader> Get(std::string_view name);
		bool Exists(std::string_view name) const ;
	private :
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}