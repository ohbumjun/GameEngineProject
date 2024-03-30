#include <Hazel.h>

/*Entry Point -> Application 프로젝트 한 곳에서만 include 해야 한다.
따라서main app 이 있는 곳으로 include 처리한다.
*/
#include "Hazel/Core/EntryPoint.h"
#include "Layer/ChatServerLayer.h"
#include "Layer/EchoTCPServerLayer.h"

class EchoTCPServerApp : public Hazel::Application
{
public:
    EchoTCPServerApp(const Hazel::ApplicationSpecification &specification)
        : Hazel::Application(specification)
    {
        // PushLayer(new ChatServerLayer());
        PushLayer(new EchoTCPServerLayer());
    }

    ~EchoTCPServerApp()
    {
    }
};

Hazel::Application *Hazel::CreateApplication(
    Hazel::ApplicationCommandLineArgs args)
{
    Hazel::ApplicationSpecification spec;
    spec.Name = "Sandbox";
    spec.WorkingDirectory = "IMGUIChat";
    spec.CommandLineArgs = args;

    return new EchoTCPServerApp(spec);
}