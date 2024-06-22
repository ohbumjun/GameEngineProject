
#include <Hazel.h>

/*Entry Point -> Application 프로젝트 한 곳에서만 include 해야 한다.
따라서main app 이 있는 곳으로 include 처리한다.
*/
#include "EditorLayer.h"
#include "File/FileManager.h"
#include "Hazel/Core/EntryPoint.h"

namespace Hazel
{
class EditorApplication : public Hazel::Application
{
public:
    EditorApplication(ApplicationCommandLineArgs args)
        : Hazel::Application(
              Hazel::ApplicationSpecification{"Hazelnut", "", args})
    {
        // PushLayer(new ExampleLayer());
        PushLayer(new HazelEditor::EditorLayer());

        // Client 측에서 ImGuiLayer 를 세팅하게 해주고 싶지 않다.
        // Engine 측 Application 에서 추가하게 할 것이다.
        // PushOverlay(new Hazel::ImGuiLayer());

        HazelEditor::FileManager::Initialize(args[0].c_str());
    }

    ~EditorApplication()
    {
    }
};

Hazel::Application *CreateApplication(ApplicationCommandLineArgs args)
{
    return new EditorApplication(args);
}
} // namespace Hazel
