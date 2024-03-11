#pragma once

#include "Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Hazel
{
class OpenGLContext : public GraphicsContext
{
public:
    OpenGLContext(GLFWwindow *windowHandle);

    // OpenGL 을 사용하기 위한 환경을 마련하는 함수
    virtual void Init() override;
    virtual void SwapBuffers() override;

private:
    GLFWwindow *m_WindowHandle;
};
}; // namespace Hazel