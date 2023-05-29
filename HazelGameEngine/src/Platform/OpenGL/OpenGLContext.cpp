#include "hzpch.h"
#include "OpenGLContext.h"

// not open gl specific, also support vulkan
#include <GLFW/glfw3.h>

// open gl specific !
#include <glad/glad.h>

namespace Hazel
{
	OpenGLContext::OpenGLContext(GLFWwindow* pWindowHandle)
	{
		m_WindowHandle = pWindowHandle;
	}
	void OpenGLContext::Init()
	{
		HZ_PROFILE_FUNCTION();

		// make context current
		glfwMakeContextCurrent(m_WindowHandle);

		// Init GLAD (ex. load extension for open gl)
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		HZ_CORE_ASSERT(status, "Faild to initialize Glad !");
	
		// tell us what gpu we are using
		HZ_CORE_INFO("OpenGL Renderer : {0}", fmt::ptr(glGetString(GL_RENDERER)));
	}
	void OpenGLContext::SwapBuffers()
	{
		HZ_PROFILE_FUNCTION();

		// refresh window 
		glfwSwapBuffers(m_WindowHandle);
	}
}