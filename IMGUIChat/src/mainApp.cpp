#include <Hazel.h>

/*Entry Point -> Application 프로젝트 한 곳에서만 include 해야 한다.
따라서main app 이 있는 곳으로 include 처리한다.
*/
#include "Hazel/Core/EntryPoint.h"
#include "./Layer/ChatLayer.h"

class Sandbox : public Hazel::Application
{
public:
    Sandbox(const Hazel::ApplicationSpecification &specification)
        : Hazel::Application(specification)
    {
        // PushLayer(new ExampleLayer());
        PushLayer(new IMGUIChatLayer());

        // Client 측에서 ImGuiLayer 를 세팅하게 해주고 싶지 않다.
        // Engine 측 Application 에서 추가하게 할 것이다.
        // PushOverlay(new Hazel::ImGuiLayer());
    }

    ~Sandbox()
    {
    }
};

Hazel::Application *Hazel::CreateApplication(
    Hazel::ApplicationCommandLineArgs args)
{
    Hazel::ApplicationSpecification spec;
    spec.Name = "Sandbox";
    spec.WorkingDirectory = "../Hazelnut";
    spec.CommandLineArgs = args;

    return new Sandbox(spec);
}