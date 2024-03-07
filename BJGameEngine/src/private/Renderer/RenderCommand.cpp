#include "Renderer/RenderCommand.h"
#include "hzpch.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Hazel
{
RendererAPI *RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}
