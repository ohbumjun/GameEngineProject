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
		// make context current
		glfwMakeContextCurrent(m_WindowHandle);

		// Init GLAD (ex. load extension for open gl)
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		HZ_CORE_ASSERT(status, "Faild to initialize Glad !");
	}
	void OpenGLContext::SwapBuffers()
	{
		// refresh window 
		glfwSwapBuffers(m_WindowHandle);
	}
}