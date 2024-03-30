#include <Hazel.h>

/*Entry Point -> Application 프로젝트 한 곳에서만 include 해야 한다.
따라서main app 이 있는 곳으로 include 처리한다.
*/
#include "Hazel/Core/EntryPoint.h"
#include "Layer/ChatServerLayer.h"
#include "Layer/EchoTCPServerLayer.h"
#include "ServerInfo.h"

// temp 변수
std::map<std::string, NetworkType> stringToEnum = {
    {"CLIENT", NetworkType::CLIENT},
    {"SERVER", NetworkType::SERVER},
    {"ECO_CLIENT", NetworkType::ECO_CLIENT},
    {"ECO_SERVER", NetworkType::ECO_SERVER}
};

class EchoTCPClientApp : public Hazel::Application
{
public:
    EchoTCPClientApp(const Hazel::ApplicationSpecification &specification)
        : Hazel::Application(specification)
    {
        // PushLayer(new ChatServerLayer());
        PushLayer(new EchoTCPServerLayer());
    }

    ~EchoTCPClientApp()
    {
    }
};

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
    spec.Name = "IMGUIChat";
    spec.WorkingDirectory = "";
    spec.CommandLineArgs = args;

    NetworkType netType;

    if (args.Count > 1)
    {
        std::string netTypeStr = args[1];

        if (stringToEnum.find(netTypeStr) != stringToEnum.end())
        {
            netType = stringToEnum[netTypeStr];
        }
        else
        {
            HZ_CORE_ASSERT(false, "Wrong Network Type");
        }
	}
    else
    {
        netType = NetworkType::ECO_SERVER;
    }
    
    switch (netType)
    {
    case NetworkType::SERVER:
        break;
    case NetworkType::CLIENT:
        break;
    case NetworkType::ECO_SERVER:
        return new EchoTCPServerApp(spec);
    case NetworkType::ECO_CLIENT:
        return new EchoTCPClientApp(spec);
    default:
        break;
    }

}