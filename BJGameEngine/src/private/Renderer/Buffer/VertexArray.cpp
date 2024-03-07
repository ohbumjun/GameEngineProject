#include "Renderer/Buffer/VertexArray.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Renderer/Renderer.h"
#include "Renderer/RendererAPI.h"
#include "hzpch.h"

namespace Hazel
{
Ref<VertexArray> VertexArray::Create()
{
    switch (Renderer::GetAPI())
    {
    case RendererAPI::API::None:
        HZ_CORE_ASSERT(false, "no api set");
        return nullptr;
    case RendererAPI::API::OpenGL:
        return std::make_shared<OpenGLVertexArray>();
    }

    HZ_CORE_ASSERT(false, "Unknown RendererAPI");
    return nullptr;
}
} // namespace Hazel
