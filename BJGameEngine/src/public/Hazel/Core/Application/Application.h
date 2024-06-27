#pragma once

#include "Hazel/Resource/Image/SubTexture2D.h"
#include "Hazel/Resource/Image/Texture.h"
#include "Hazel/Resource/Shader/Shader.h"
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
    ApplicationCommandLineArgs(int count, char **Args) :
        m_Count(count)
    {
        for (int i = 0; i < count; i++)
        {
            m_Args.push_back(Args[i]);
        }
    }
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

class HAZEL_API ApplicationContext
{
public:
    // Application 사용 폴더명 모음.
    struct Directories
    {
        /*
        * builtin 폴더
		 */
        static const char *defaultAssets;
    };

    struct ResourceDirectories
    {
        static const char *fonts;
        static const char *shaders;
        static const char *textures;
    };

    ApplicationContext(const std::string& name, 
        const ApplicationCommandLineArgs &commandLineArgs)
		: m_Name(name), m_CommandLineArgs(commandLineArgs)
	{
        initialize();
	}

    const ApplicationCommandLineArgs& GetCommandLineArgs() const
	{
        return m_CommandLineArgs;
	}

    const std::string& GetName() const
    {
        return m_Name;
    }

    const std::string &GetDefaultAssetPath()
    {
        return m_DefaultAssetsPath;
    }

   const  std::string& GetResourceRootPath()
    {
        return m_ResourceRootPath;
    }

const std::string& GetExecutableDirectoryPath()
	{
		return m_ExecutableDirectoryPath;
	}

private:
    void initialize();

    std::string m_Name = "Hazel Application";
    // @brief Default Asset 의 경로
    std::string m_DefaultAssetsPath;
    // @brief BJResource 와 같이 Resource 들이 담긴 최상위 경로
    std::string m_ResourceRootPath;
    // @brief 실행 파일이 있는 경로
    std::string m_ExecutableDirectoryPath;
    ApplicationCommandLineArgs m_CommandLineArgs;
};

/*
* Application Class 가 사실상 Engine 의 역할을 한다.
* 따라서 Engine 혹은 EngineContext 가 필요한지는 조금 더 생각해봐야 한다.
*/
class HAZEL_API Application
{
public:
    Application(const ApplicationContext &specification);
    virtual ~Application();
    virtual void Run();
    void Close();
    void OnEvent(Event &e);
    void Finalize();
    virtual void Initialize();
    // @brief Editor 혹은 Engine Layer 를 추가한 이후, 별도로 초기화 하기 위해 함수 구분
    void initImgui();
    void PushLayer(Layer *layer);
    void PushOverlay(Layer *layer);
    void PopLayer(Layer *layer);

    ImGuiLayer *GetImGuiLayer() const
    {
        return m_ImGuiLayer;
    };
    const ApplicationContext &GetSpecification() const
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
    ApplicationContext m_Specification;
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
