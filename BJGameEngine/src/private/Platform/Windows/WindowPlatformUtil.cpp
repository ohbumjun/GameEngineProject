#include "Hazel/Utils/PlatformUtils.h"
#include "hzpch.h"
#include <commdlg.h>

#include <GLFW/glfw3.h>

/*
Specifically, when this directive is defined before including
the GLFW header file,
it allows you to access platform-specific native handles,
such as the HWND for Windows, NSWindow for macOS, etc.,
directly from the GLFW API.
*/
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "Hazel/Core/Application/Application.h"

namespace Hazel
{
float Time::GetTime()
{
    return glfwGetTime();
}
std::string FileChooser::OpenFile(const char *filter)
{
    OPENFILENAMEA ofn;
    CHAR szFile[260] = {0};
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = glfwGetWin32Window(
        (GLFWwindow *)Application::Get().GetWindow().GetNativeWindow());
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn) == TRUE)
    {
        return ofn.lpstrFile;
    }

    // 아무것도 클릭안하고, 그냥 창을 닫으면 여기에 걸린다.
    // assert(false);

    return std::string();
}

std::string FileChooser::SaveFile(const char *filter)
{
    OPENFILENAMEA ofn;
    CHAR szFile[260] = {0};
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = glfwGetWin32Window(
        (GLFWwindow *)Application::Get().GetWindow().GetNativeWindow());
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetSaveFileNameA(&ofn) == TRUE)
    {
        return ofn.lpstrFile;
    }

    // 아무것도 클릭안하고, 그냥 창을 닫으면 여기에 걸린다.
    // assert(false);
    return std::string();
}
} // namespace Hazel
