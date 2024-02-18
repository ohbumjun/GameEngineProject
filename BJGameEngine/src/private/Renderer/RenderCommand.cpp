#include "hzpch.h"
#include "Renderer/RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Hazel
{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}
