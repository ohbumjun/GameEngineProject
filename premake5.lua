workspace "HazelGameEngine" --솔루션파일 이름
    architecture "x64"      --솔루션의 architecture, 32bit인지 64bit인지 설정

    configurations          --구성 (debug모드, release모드 등 어떤 구성이 있는지?)
    {
        "Debug",
        "Release",
        "Dist"
    }

--결과물 폴더경로를 outputdir변수에 저장
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- root folder / solution directory 와 관련된 directory 들
IncludeDir = {}
IncludeDir["GLFW"] = "HazelGameEngine/vendor/GLFW/include"

-- 해당 경로 내의 premake.lua 파일을 include => 그대로 복사붙여넣기 한다는 것이다. 여기 lua file 에
include "HazelGameEngine/vendor/GLFW"

project "HazelGameEngine"       --프로젝트 이름
    location "HazelGameEngine"
    kind "SharedLib"            
    language "C++"              

    --생성파일(exe,lib,dll) 경로설정
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")  
    --obj파일경로 설정
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}") 

    pchheader "hzpch.h"
    pchsource "HazelGameEngine/src/hzpch.cpp"

    files   --어떤파일을 컴파일 할 것인지?
    {
        "%{prj.name}/src/**.h", -- 프로젝트이름폴더-> src폴더안에있는 모든 헤더파일들
        "%{prj.name}/src/**.cpp"
    }

    defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

    includedirs                 --추가포함 디렉토리경로 설정
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}"
    }

    -- static library 를 link 시킨다.
    links
    {
        "GLFW",
        "opengl32.lib"
    }

    filter "system:windows"     -- 특정환경에 대한 설정 (ex window환경 )
            cppdialect "C++17"
            staticruntime "on"
            systemversion "latest"   --윈도우버전을 최신으로 설정

        defines                    
        {
            "HZ_PLATFORM_WINDOWS",  --Hazel프로젝트에는 이러한 전처리가 있다.
            "HZ_BUILD_DLL"
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/ApplicationProject/\"")
        }

    filter "configurations:Debug" --디버그구성일 때 설정
        defines "HZ_DEBUG"
        buildoptions "/MDd" -- C/C++ -> CodeGeneration -> RunTime Library -> MT 세팅
        symbols "On"

    filter "configurations:Release" 
        defines "HZ_RELEASE"
        buildoptions "/MDd"
        symbols "On"

    filter "configurations:Dist"
        defines "HZ_DIST"
        buildoptions "/MDd"
        symbols "On"

project "ApplicationProject"
    location "ApplicationProject"
    kind "ConsoleApp"
    language "C++"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs 
    {
        "HazelGameEngine/vendor/spdlog/include",
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
            systemversion "latest"

    filter "configurations:Debug"
        defines "HZ_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "HZ_RELEASE"
        symbols "On"

    filter "configurations:Dist"
        defines "HZ_DIST"
        symbols "On"