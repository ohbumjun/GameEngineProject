
#include <Hazel.h>

/*Entry Point -> Application 프로젝트 한 곳에서만 include 해야 한다.
따라서main app 이 있는 곳으로 include 처리한다.
*/
#include "EditorLayer.h"
#include "ProjectSelectLayer.h"
#include "Hazel/Core/EntryPoint.h"
#include "Hazel/Resource/DefaultResourceManager.h"
#include "EditorContext.h"
#include "Editor.h"
#include "Hazel/Core/EngineContext.h"

namespace Hazel
{
class EditorApplication : public Hazel::Application
{
public:
    EditorApplication(Hazel::ApplicationCommandLineArgs args) 
        : Hazel::Application(Hazel::ApplicationContext{"BJEditor", args})
    {
    }

    virtual void Initialize() override
    {
        // Application (Engine) 생성자 안에서 Specification 을 설정

        // 필요하다면, Audio 관련 초기화도 Application 생성자 안에서 처리

        // Editor 생성 및 EditorContext 관련 초기화
        m_Editor = HazelEditor::EditorContext::Initialize();

        // 기본 세팅
        DefaultResourceManager::LoadDefaultResources();

        // Application 
        Application::Initialize();

         PushLayer(new HazelEditor::ProjectSelectLayer());
    }

    virtual void Run()
    {
    }

    void Finalize()
    {
    }

    ~EditorApplication()
    {
    }

 private:
    HazelEditor::Editor *m_Editor
};

Application *CreateApplication(ApplicationCommandLineArgs args)
{
    Application * app = new EditorApplication(args);
    app->Initialize();
    return app;
}
} // namespace Hazel
