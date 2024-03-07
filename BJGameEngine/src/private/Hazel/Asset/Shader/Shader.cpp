
#include "Hazel/Asset/Shader/Shader.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Renderer/Renderer.h"
#include "Renderer/RendererAPI.h"
#include "hzpch.h"

namespace Hazel
{
Ref<Shader> Shader::Create(std::string_view path)
{
    switch (Renderer::GetAPI())
    {
    case RendererAPI::API::None:
        HZ_CORE_ASSERT(false, "no api set");
        return nullptr;
    case RendererAPI::API::OpenGL:
        return std::make_shared<OpenGLShader>(path);
    }

    HZ_CORE_ASSERT(false, "Unknown RendererAPI");
    return nullptr;
}
Ref<Shader> Shader::Create(std::string_view name,
                           const std::string &vertexSrc,
                           const std::string &fragmentSrc)
{
    switch (Renderer::GetAPI())
    {
    case RendererAPI::API::None:
        HZ_CORE_ASSERT(false, "no api set");
        return nullptr;
    case RendererAPI::API::OpenGL:
        return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
    }

    HZ_CORE_ASSERT(false, "Unknown RendererAPI");
    return nullptr;
}

void ShaderLibrary::Add(std::string_view name, const Ref<Shader> &shader)
{
    HZ_CORE_ASSERT(Exists(name) == false, "Shader Alreaad Exist");
    m_Shaders[name.data()] = shader;
}

void ShaderLibrary::Add(const Ref<Shader> &shader)
{
    std::string_view name = shader->GetName();
    Add(name, shader);
}

Ref<Shader> ShaderLibrary::Load(std::string_view filePath)
{
    auto shader = Shader::Create(filePath);
    Add(shader);
    return shader;
}
Ref<Shader> ShaderLibrary::Load(std::string_view name,
                                std::string_view filePath)
{
    auto shader = Shader::Create(filePath);
    Add(name, shader);
    return shader;
}
Ref<Shader> ShaderLibrary::Get(std::string_view name)
{
    HZ_CORE_ASSERT(Exists(name), "Shader Does not Exist");
    return m_Shaders[name.data()];
}
bool ShaderLibrary::Exists(std::string_view name) const
{
    return m_Shaders.find(name.data()) != m_Shaders.end();
}
} // namespace Hazel
