#pragma once

#include "Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Hazel
{
class OpenGLContext : public GraphicsContext
{
public:
    OpenGLContext(GLFWwindow *windowHandle);

    // OpenGL �� ����ϱ� ���� ȯ���� �����ϴ� �Լ�
    virtual void Init() override;
    virtual void SwapBuffers() override;

private:
    GLFWwindow *m_WindowHandle;
};
}; // namespace Hazel