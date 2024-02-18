# CONFIGURE_DEPENDS : when file is added or removed, cmake will be reconfigured
# set(ENGINE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/private/**.cpp")
file(
    GLOB_RECURSE 
    ENGINE_HEADERS 
    CONFIGURE_DEPENDS 
"${CMAKE_CURRENT_SOURCE_DIR}/*.h")

file(
    GLOB_RECURSE 
    ENGINE_SOURCES 
"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

file(
    GLOB 
    ENGINE_EXTERNAL_SOURCES 
    "${CMAKE_SOURCE_DIR}/external/stb_image/*.cpp"
    "${CMAKE_SOURCE_DIR}/external/stb_image/*.h"
    "${CMAKE_SOURCE_DIR}/external/glm/glm/*.hpp"
    "${CMAKE_SOURCE_DIR}/external/ImGuizmo/ImGuizmo.h"
    "${CMAKE_SOURCE_DIR}/external/ImGuizmo/ImGuizmo.cpp"
)

# is this lib file ?
# Set the output directory for the binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/${EDITOR_NAME})

# is this obj file ?
# Set the output directory for the intermediate files
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin-int/${CMAKE_BUILD_TYPE}/${EDITOR_NAME})


set(ENGINE_INCLUDES
    "./"
    "${CMAKE_SOURCE_DIR}/external/spdlog/include"
    "${CMAKE_SOURCE_DIR}/external/GLFW/include"
    "${CMAKE_SOURCE_DIR}/external/Glad/include"
    "${CMAKE_SOURCE_DIR}/external/imgui"
    "${CMAKE_SOURCE_DIR}/external/glm"
    "${CMAKE_SOURCE_DIR}/external/stb_image"
    "${CMAKE_SOURCE_DIR}/external/entt/include"
    "${CMAKE_SOURCE_DIR}/external/RapidJson"
    "${CMAKE_SOURCE_DIR}/external/ImGuizmo"
    "${CMAKE_SOURCE_DIR}/external/Box2D/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/public" 
    )

# Create filters for Visual Studio
foreach(FILE ${ENGINE_SOURCES} ${ENGINE_HEADERS}) 
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")

    # Group into "Source Files" and "Header Files"
    if ("${FILE}" MATCHES ".*\\.cpp")
       set(GROUP "Source Files${GROUP}")
    elseif("${FILE}" MATCHES ".*\\.h")
       set(GROUP "Header Files${GROUP}")
    endif()

    source_group("${GROUP}" FILES "${FILE}")
endforeach()

# create target, but not executable == library
# STATIC : staic library    ex) .lib
# SHARED : dynamic library  ex) .dll
add_library(
    ${ENGINE_NAME} 
    SHARED 
    ${ENGINE_HEADERS}
    ${ENGINE_SOURCES}
    )

# Add precompiled header source file
# target_sources(${ENGINE_NAME} PRIVATE "hzpch.cpp")
target_precompile_headers(
    ${ENGINE_NAME} 
    PUBLIC 
    # "${CMAKE_CURRENT_SOURCE_DIR}/HazelGameEngine/src/hzpch.h"
    "hzpch.h"
)

target_compile_definitions(
    ${ENGINE_NAME} 
    PRIVATE 
    _CRT_SECURE_NO_WARNINGS 
    GLFW_INCLUDE_NONE
    _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
    )
# where to find header files
# cpp file can include header files from "./~~"
target_include_directories(
    ${ENGINE_NAME} 
    PUBLIC 
    ${ENGINE_INCLUDES})

# link library to own library
target_link_libraries(
    ${ENGINE_NAME}
    PUBLIC # other libraries
           # ex) projectname::libraryname (refer to other github cmakelists.txt)
    box2d  # find project name in actual cmakefile
    imgui
    glfw
    Glad
    glm
    opengl32.lib
    stb_image
    )

# 아래 두 줄을 통해서 해당 경로에 있는 모든 cpp 파일들을 pch 를 사용하지 않을 것이라고 하는 것이다
# 그러면 hazel engine 은 현재 hzpch.cpp 를 무조건 모든 cpp 파일에서 include 하게 해놨는데
# 이것을 imgui 코드들은 하지 않도록 하는 것이다.
set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/external/ImGuizmo/**.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stb_image/**.cpp
    PROPERTIES COMPILE_FLAGS "/Yc;pch.h"
)

# Windows specific settings
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    # Set the system version to the latest
    set(CMAKE_SYSTEM_VERSION "latest")

    # Define Hazel specific preprocessor macros
    target_compile_definitions(
        ${ENGINE_NAME} 
        PRIVATE
        HZ_PROFILE
        HZ_PLATFORM_WINDOWS
        HZ_BUILD_DLL
        GLFW_INCLUDE_NONE
    )
endif()


# Debug configuration
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_definitions(${ENGINE_NAME} 
    PUBLIC 
    HZ_DEBUG)

    # game engine을 dll 로 만드는 경우 세팅
    # /MDd for MT runtime library in Debug
    target_compile_options(${ENGINE_NAME} 
    PUBLIC 
    /MDd)  # For MT runtime library

    # DEBUG_POSTFIX: Appends "d" 
    # to the library name in Debug configuration.
    set_target_properties(
        ${ENGINE_NAME} 
    PROPERTIES 
    DEBUG_POSTFIX "d")

    set_target_properties(
        ${ENGINE_NAME} 
    PROPERTIES 
    # RUNTIME_OUTPUT_DIRECTORY_DEBUG ?
    # = Specifies output directory for Debug binaries.
    RUNTIME_OUTPUT_DIRECTORY_DEBUG 
    "${CMAKE_BINARY_DIR}/bin/Debug")

endif()

# Release configuration
if(CMAKE_BUILD_TYPE MATCHES "Release")
    target_compile_definitions(
        ${ENGINE_NAME} 
        PUBLIC 
        HZ_RELEASE)
    set_target_properties(
        ${ENGINE_NAME} 
        PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY_RELEASE 
        "${CMAKE_BINARY_DIR}/bin/Release")
endif()

# Dist configuration
if(CMAKE_BUILD_TYPE MATCHES "Dist")
    target_compile_definitions(
        ${ENGINE_NAME}  
        PUBLIC 
        HZ_DIST)
    set_target_properties(
        ${ENGINE_NAME}  
        PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE 
        "${CMAKE_BINARY_DIR}/bin/Dist")
endif()