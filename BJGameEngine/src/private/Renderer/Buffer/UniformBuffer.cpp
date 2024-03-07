#include "Renderer/Buffer/UniformBuffer.h"
#include "hzpch.h"

#include "Platform/OpenGL/OpenGLUniformBuffer.h"
#include "Renderer/Renderer.h"

namespace Hazel
{

Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
{
    switch (Renderer::GetAPI())
    {
    case RendererAPI::API::None:
        HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL:
        return CreateRef<OpenGLUniformBuffer>(size, binding);
    }

    HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

} // namespace Hazel