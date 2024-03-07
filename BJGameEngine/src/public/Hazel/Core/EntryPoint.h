#pragma once

#ifdef HZ_PLATFORM_WINDOWS

// 외부에 정의된 함수이고, Application 을 리턴해준다.
extern Hazel::Application *Hazel::CreateApplication(
    Hazel::ApplicationCommandLineArgs args);

int main(int argc, char **argv)
{
    /*Log Init*/
    Hazel::Log::Init();

    Hazel::Log::GetCoreLogger()->warn("Initialized Log");

    Hazel::Log::GetClientLogger()->info("Hello");

    HZ_PROFILE_BEGIN_SESSION("Startup", "HazelProfile-Startup.json");
    auto *app = Hazel::CreateApplication({argc, argv});
    HZ_PROFILE_END_SESSION();

    HZ_PROFILE_BEGIN_SESSION("Runtime", "HazelProfile-Runtime.json");
    app->Run();
    HZ_PROFILE_END_SESSION();

    HZ_PROFILE_BEGIN_SESSION("Delete", "HazelProfile-Delete.json");
    delete app;
    HZ_PROFILE_END_SESSION();
}
#endif