#pragma once

#include "Hazel/Asset/Image/SubTexture2D.h"
#include "Hazel/Asset/Image/Texture.h"
#include "Hazel/Asset/Shader/Shader.h"
#include "Hazel/Core/Core.h"
#include "Hazel/Core/Layer/LayerStack.h"
#include "Hazel/Event/ApplicationEvent.h"
#include "Hazel/Event/Event.h"
#include "Hazel/ImGui/ImGuiLayer.h"
#include "Window.h"
#include "Hazel/Utils/TimeStep.h"


// Application 의 실행을 Application Project 가 아니라
// Engine 단에서 제어하고 싶은 것
namespace Hazel
{
struct HAZEL_API ApplicationCommandLineArgs
{
public:
    ApplicationCommandLineArgs() = default;

    ApplicationCommandLineArgs(int count, char **Args);

    const std::string& operator[](int index) const
    {
        HZ_CORE_ASSERT(index < m_Count, "index out of range");
        return m_Args[index];
    }

    inline int GetCount()
    {
		return m_Count;
    }
    inline int GetCount() const
    {
        return m_Count;
    }

private:
    int m_Count = 0;
    std::vector<std::string> m_Args;

};
struct HAZEL_API ApplicationSpecification
{

    std::string Name = "Hazel Application";
    std::string WorkingDirectory = "";
    ApplicationCommandLineArgs CommandLineArgs;
};
// _declspec(dllexport) class Application
class HAZEL_API Application
{
public:
    // Application(const std::string& name = "DefaultName");
    Application(const ApplicationSpecification &specification);
    virtual ~Application();
    void Run();
    void Close();
    void OnEvent(Event &e);
    void Finalize();
    void Initialize();
    void PushLayer(Layer *layer);
    void PushOverlay(Layer *layer);
    void PopLayer(Layer *layer);

    ImGuiLayer *GetImGuiLayer() const
    {
        return m_ImGuiLayer;
    };
    const ApplicationSpecification &GetSpecification() const
    {
        return m_Specification;
    }
    inline Window &GetWindow()
    {
        return *m_Window;
    }

    inline static Application &Get()
    {
        return *s_Instance;
    }

    inline class ThreadPool *GetMainThreadPool();

private:
    bool OnWindowClose(WindowCloseEvent &e);
    bool OnWindowResize(WindowResizeEvent &e);

    // unique ptr 로 세팅해줌으로써 소멸자에서 별도로 소멸시켜줄 필요가 없다.
    ApplicationSpecification m_Specification;
    std::unique_ptr<Window> m_Window;
    ImGuiLayer *m_ImGuiLayer;
    bool m_Running = true;
    bool m_Minimized = false;
    LayerStack m_LayerStack;
    Timestep m_Timestep;
    float m_LastFrameTime = 0.f;

private:
    static Application *s_Instance;
};

// To be defined in Client
Application *CreateApplication(Hazel::ApplicationCommandLineArgs args);
}; // namespace Hazel
