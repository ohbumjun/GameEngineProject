#include "hzpch.h"
#include "imgui.h"

// 참고 : 정상적으로 아래의 h 파일들의 정보를 사용할 수 있는 이유는
// imbuibuild.cpp 에 해당 h 파일들에 대한 cpp 파일들도
// 컴파일할 수 있게 정보를 세팅해두었기 때문이다.
#include "Hazel/Core/Application/Application.h"
#include "Hazel/Core/EngineContext.h"
#include "ImGuizmo.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// TEMPORARY
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "Hazel/ImGui/ImGuiContextManager.h"
#include "Hazel/Core/Allocation/Allocator/FreeListAllocator.h"

namespace Hazel
{
FreeListAllocator *s_imguiAllocator = new FreeListAllocator(1024 * 1024);

static void *imgui_malloc(size_t sz, void *user_data)
{
    return s_imguiAllocator->Allocate(sz, __FILE__, __LINE__);
    // return lv_malloc(sz);
}

static void imgui_free(void *ptr, void *user_data)
{
    s_imguiAllocator->Free(ptr);
    // lv_free(ptr);
}

void ImguiContextManager::setDarkThemeColor()
{
    auto &colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.105f, 0.11f, 1.0f};

    // Headers
    colors[ImGuiCol_Header] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_HeaderHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_HeaderActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_ButtonHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_ButtonActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_FrameBgHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_FrameBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TabHovered] = ImVec4{0.38f, 0.3805f, 0.381f, 1.0f};
    colors[ImGuiCol_TabActive] = ImVec4{0.28f, 0.2805f, 0.281f, 1.0f};
    colors[ImGuiCol_TabUnfocused] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TitleBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
}

ImguiContextManager::ImguiContextManager()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
}

ImguiContextManager::~ImguiContextManager()
{
}

void ImguiContextManager::Initialize()
{
    ImGui::SetAllocatorFunctions(imgui_malloc, imgui_free);
}

void ImguiContextManager::Finalize()
{
}

ImGuiContext *ImguiContextManager::CreateContext()
{
    // static std::string resourceRootPath = RESOURCE_ROOT;

    auto applicationContext = Application::Get().GetSpecification();
    const std::string &resourceRootPath =
        applicationContext.GetDefaultAssetPath();

    ImGuiContext * imguiContext = ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    io.ConfigFlags |=
        ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

    // Font
    // 1 : regular / 2 : bold

    float fontSize = 18.0f; // *2.0f;
    char openSansBoldPath[200];
    char openSansRegularPath[200];

    strcpy(openSansBoldPath, resourceRootPath.c_str()); // Copy str1 to result
    strcat(openSansBoldPath, ApplicationContext::ResourceDirectories::fonts);
    strcat(openSansBoldPath, "/opensans/OpenSans-Bold.ttf");

    strcpy(openSansRegularPath,
           resourceRootPath.c_str()); // Copy str1 to result
    strcat(openSansRegularPath, ApplicationContext::ResourceDirectories::fonts);
    strcat(openSansRegularPath, "/opensans/OpenSans-Regular.ttf");

    io.Fonts->AddFontFromFileTTF(openSansBoldPath, fontSize);
    io.FontDefault =
        io.Fonts->AddFontFromFileTTF(openSansRegularPath, fontSize);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    setDarkThemeColor();

    Application &app = Application::Get();
    GLFWwindow *window =
        static_cast<GLFWwindow *>(app.GetWindow().GetNativeWindow());

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");

    return imguiContext;
}

void ImguiContextManager::DestroyContext(ImGuiContext *context)
{
    ImGui::DestroyContext(context);
}

} // namespace Hazel