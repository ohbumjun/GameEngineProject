workspace "HazelGameEngine"
    architecture "x64" 

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "${cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "HazelGameEngine"
    location "HazelGameEngine"
    kind "SharedLib"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/vendor/spdlog/include"
    }

    filter "system:windows"
            cppdialect "C++17"
            staticruntime "on"
            systemversion "10.0.19041.0"

        defines
        {
            "HZ_PLATFORM_WINDOWS",
            "HZ_BUILD_DLL"
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "ApplicationProject/\"")
        }

    filter "configurations:Debug"
        defines "HZ_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "HZ_RELEASE"
        symbols "On"

    filter "configurations:Dist"
        defines "HZ_DIST"
        symbols "On"

project "ApplicationProject"
    location "ApplicationProject"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{proj.name}/src/**.cpp"
    }

    includedirs 
    {
        "%{proj.name}/vendor/spdlog/include",
        "HazelGameEngine/src"
    }
    
    links
    {
        "HazelGameEngine"
    }

    defines
    {
        "HZ_PLATFORM_WINDOWS"
    }

    filter "system:windows"
            cppdialect "C++17"
            staticruntime "On"
            systemversion "10.0.19041.0"

    filter "configurations:Debug"
        defines "HZ_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "HZ_RELEASE"
        symbols "On"

    filter "configurations:Dist"
        defines "HZ_DIST"
        symbols "On"