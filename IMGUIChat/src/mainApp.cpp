#include <Hazel.h>

/*Entry Point -> Application 프로젝트 한 곳에서만 include 해야 한다.
따라서main app 이 있는 곳으로 include 처리한다.
*/
#include "Hazel/Core/EntryPoint.h"
#include "Layer/ChatServerLayer.h"
#include "Layer/EchoTCPServerLayer.h"
#include "Layer/EchoTCPClientLayer.h"
#include "Layer/EchoUDPServerLayer.h"
#include "Layer/EchoUDPClientLayer.h"
#include "ServerInfo.h"

// temp 변수
std::map<std::string, NetworkType> stringToEnum = {
    {"CLIENT", NetworkType::CLIENT},
    {"SERVER", NetworkType::SERVER},
    {"ECO_TCP_CLIENT", NetworkType::ECO_TCP_CLIENT},
    {"ECO_TCP_CLIENT", NetworkType::ECO_TCP_SERVER},
    {"ECO_UDP_SERVER", NetworkType::ECO_UDP_SERVER},
    {"ECO_UDP_CLIENT", NetworkType::ECO_UDP_CLIENT}
};
class EchoTCPClientApp : public Hazel::Application
{
public:
    EchoTCPClientApp(const Hazel::ApplicationSpecification &specification)
        : Hazel::Application(specification)
    {
        // PushLayer(new ChatServerLayer());
        PushLayer(new EchoTCPClientLayer());
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

class EchoUDPClientApp : public Hazel::Application
{
public:
    EchoUDPClientApp(const Hazel::ApplicationSpecification &specification)
        : Hazel::Application(specification)
    {
        // PushLayer(new ChatServerLayer());
        PushLayer(new EchoUDPClientLayer());
    }

    ~EchoUDPClientApp()
    {
    }
};

class EchoUDPServerApp : public Hazel::Application
{
public:
    EchoUDPServerApp(const Hazel::ApplicationSpecification &specification)
        : Hazel::Application(specification)
    {
        // PushLayer(new ChatServerLayer());
        PushLayer(new EchoUDPServerLayer());
    }

    ~EchoUDPServerApp()
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
        // netType = NetworkType::ECO_TCP_SERVER;
        netType = NetworkType::ECO_UDP_SERVER;
    }
    
    switch (netType)
    {
    case NetworkType::SERVER:
        break;
    case NetworkType::CLIENT:
        break;
    case NetworkType::ECO_TCP_SERVER:
        return new EchoTCPServerApp(spec);
    case NetworkType::ECO_TCP_CLIENT:
        return new EchoTCPClientApp(spec);
    case NetworkType::ECO_UDP_SERVER:
        return new EchoUDPServerApp(spec);
    case NetworkType::ECO_UDP_CLIENT:
        return new EchoUDPClientApp(spec);
    default:
        break;
    }

}