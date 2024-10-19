workspace "HazelGameEngine" --솔루션파일 이름
    architecture "x64"      --솔루션의 architecture, 32bit인지 64bit인지 설정
    startproject "ApplicationProject"

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
IncludeDir["Glad"] = "HazelGameEngine/vendor/Glad/include"
IncludeDir["ImGui"] = "HazelGameEngine/vendor/imgui"
IncludeDir["glm"] = "HazelGameEngine/vendor/glm"
IncludeDir["stb_image"] = "HazelGameEngine/vendor/stb_image"
IncludeDir["entt"] = "HazelGameEngine/vendor/entt/include"
IncludeDir["rapidJson"] = "HazelGameEngine/vendor/RapidJson"
IncludeDir["ImGuizmo"] = "HazelGameEngine/vendor/ImGuizmo"
IncludeDir["Box2D"] = "HazelGameEngine/vendor/Box2D/include"

-- 해당 경로 내의 premake.lua 파일을 include => 그대로 복사붙여넣기 한다는 것이다. 여기 lua file 에
-- 관찰한 바, 아래의 include 파일들을 통해서 해당 프로젝트들이 솔루션에 추가된다.
group "Dependencies"
    include "HazelGameEngine/vendor/Box2D"
    include "HazelGameEngine/vendor/GLFW"
    include "HazelGameEngine/vendor/Glad"
    include "HazelGameEngine/vendor/imgui"
-- include "HazelGameEngine/vendor/glm" 오직 header only library 이므로 별도의 lua file 이 존재하지 않는다.

group ""

project "HazelGameEngine"       --프로젝트 이름
    location "HazelGameEngine"
    kind "SharedLib" -- static lib / kind "SharedLib" : dll      
    language "C++"
    cppdialect "C++17"
    staticruntime "off" -- static lib 일 경우 on, shared lib 일 경우 off    

    --생성파일(exe,lib,dll) 경로설정
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")  
    --obj파일경로 설정
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}") 

    pchheader "hzpch.h"
    pchsource "HazelGameEngine/src/hzpch.cpp"

    files   --어떤파일을 컴파일 할 것인지? => 이를 통해 솔루션 상에서 볼 수 있게 된다.
    {
        "%{prj.name}/src/**.h", -- 프로젝트이름폴더-> src폴더안에있는 모든 헤더파일들
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/stb_image/**.cpp",
        "%{prj.name}/vendor/stb_image/**.h",
        "%{prj.name}/vendor/glm/glm/**.hpp", -- glm 라이브러리의 각종 hpp, inl 파일들을 솔루션 탐색기상에서 볼 수 있게 하고 싶다.
        "%{prj.name}/vendor/ImGuizmo/ImGuizmo.h",
        "%{prj.name}/vendor/ImGuizmo/ImGuizmo.cpp"

    }

    defines
	{
		"_CRT_SECURE_NO_WARNINGS",
        "GLFW_INCLUDE_NONE"
	}

    includedirs                 --추가포함 디렉토리경로 설정
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.rapidJson}",
		"%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.Box2D}"

    }

    -- static library 를 link 시킨다.
    links
    {
        "Box2D",
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib"
    }

    -- 아래 두 줄을 통해서 해당 경로에 있는 모든 cpp 파일들을 pch 를 사용하지 않을 것이라고 하는 것이다
    -- 그러면 hazel engine 은 현재 hzpch.cpp 를 무조건 모든 cpp 파일에서 include 하게 해놨는데
    -- 이것을 imgui 코드들은 하지 않도록 하는 것이다.
    filter "HazelGameEngine/vendor/ImGuizmo/**.cpp"
	flags { "NoPCH" }

    filter "system:windows"     -- 특정환경에 대한 설정 (ex window환경 )
        systemversion "latest"   --윈도우버전을 최신으로 설정

        defines                    
        {
            "HZ_PROFILE",
            "HZ_PLATFORM_WINDOWS",  --Hazel프로젝트에는 이러한 전처리가 있다.
            "HZ_BUILD_DLL",
            "GLFW_INCLUDE_NONE" -- not include openGL header when include GLFW
        }

        -- dll 파일을 만드는 것이 아니기 때문에 더이상 필요없다.
        postbuildcommands
        {
           ("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/ApplicationProject/\"")
        }

    filter "configurations:Debug" --디버그구성일 때 설정
        defines "HZ_DEBUG"
        runtime "Debug"
        -- hazel game engine을 dll 로 만드는 경우 세팅 => buildoptions "/MDd" -- C/C++ -> CodeGeneration -> RunTime Library -> MT 세팅
        symbols "on" -- symbols "on" : debug version of library => generate debug symbols

    filter "configurations:Release" 
        defines "HZ_RELEASE"
        runtime "Release"
        optimize "On" -- optimize "on" : relase version of libray 


    filter "configurations:Dist"
        defines "HZ_DIST"
        runtime "Release"
        optimize "On"

project "ApplicationProject"
    location "ApplicationProject"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

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
        "HazelGameEngine/src",
        "HazelGameEngine/vendor",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.rapidJson}"
    }
    
    links
    {
        "HazelGameEngine"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "HZ_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "HZ_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "HZ_RELEASE"
        runtime "Release"
        symbols "on"

    filter "configurations:Dist"
        defines "HZ_DIST"
        runtime "Release"
        optimize "on"


project "Hazel-Editor"
    location "Hazel-Editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

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
        "HazelGameEngine/src",
        -- "Hazel-Editor/src",
        "HazelGameEngine/vendor",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.rapidJson}",
		"%{IncludeDir.ImGuizmo}"
    }
    
    links
    {
        "HazelGameEngine"
    }

    -- 아래 두 줄을 통해서 해당 경로에 있는 모든 cpp 파일들을 pch 를 사용하지 않을 것이라고 하는 것이다
    -- 그러면 hazel engine 은 현재 hzpch.cpp 를 무조건 모든 cpp 파일에서 include 하게 해놨는데
    -- 이것을 imgui 코드들은 하지 않도록 하는 것이다.
    filter "%{prj.name}/vendor/ImGuizmo/**.cpp"
	flags { "NoPCH" }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "HZ_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "HZ_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "HZ_RELEASE"
        runtime "Release"
        symbols "on"

    filter "configurations:Dist"
        defines "HZ_DIST"
        runtime "Release"
        optimize "on"