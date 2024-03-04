# set(CMAKE_CONFIGURATION_TYPES "Debug;RelWithDebInfo" CACHE STRING "" FORCE)
cmake_minimum_required(VERSION 3.22)

project(
    BumJunEngine
    VERSION 1.2.3.4
    LANGUAGES C CXX
    # "www.naver.com"
)

set(CMAKE_CXX_STANDARD 17)

# 위의 standard version 이 지원될 때에만, 컴파일이 되도록 하기 
set(CMAKE_CXX_STANDARD_REQUIRED On)

# ex) some c variable not in C++ standard. but can be used in gcc
#     disable these kinds of examples
#     즉, 특정 컴파일러에서 제공하는 옵션을 사용할 것인가 아닌다
#     ex) filesystem : C++17 에서 제공하는 표준 라이브러리. 그 이전에는 additional 기능으로만 존재
#           이와 같은 실험적, additional 기능을 사용할 것인가 아닌가를 결정
set(CMAKE_CXX_EXTENSIONS OFF)

# generate 'compile_commands.json' file so that clang-tidy can know
# which files to look for
# ex) clang-tidy is a tool that runs before compilation
#     so it has to know where to look for all files of project
#     based on compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

## -- Useful Predifined Variables -- ##
# OS Name ex) Darwin(MacOS), Linux, Windows
message(STATUS "OS Name : ${CMAKE_SYSTEM_NAME}")

# OS Version
message(STATUS "OS Version : ${CMAKE_SYSTEM_VERSION}") 

# OS Name - OS Version
message(STATUS "OS Name-OS Version : ${CMAKE_SYSTEM}")  

# CPU-Architecture ex) x86, x86_64, arm, arm64
message(STATUS "CPU Architecture : ${CMAKE_SYSTEM_PROCESSOR}")  

# CMake 를 "run" 하는 시스템의 이름
# ex) Cross -Compiling 을 할 때, Build 시스템과 Run 시스템이 다를 수 있다.
# ex) CMAKE_SYSTEM 은 "Build" 시스템의 이름을 나타낸다.
# message(STATUS "CMAKE_HOST_SYSTEM               : ${CMAKE_HOST_SYSTEM}")  
# message(STATUS "CMAKE_HOST_SYSTEM_NAME      : ${CMAKE_HOST_SYSTEM_NAME}")  
# message(STATUS "CMAKE_HOST_SYSTEM_VERSION : ${CMAKE_HOST_SYSTEM_VERSION}")  
# message(STATUS "CMAKE_HOST_SYSTEM_PROCESSOR : ${CMAKE_HOST_SYSTEM_PROCESSOR}")  

# PC 에 설치되어 있는 'C' 컴파일러의 Executable 을 이용하여 컴파일을 한다. 
# - 정확하게는 현재 CMake 를 빌드하는 컴파일러 Executable 경로 
# - 만약 다른 컴파일러로 CMake 를 빌드하겠다고 선택하면 아래의 값도 달라질 것이다.
# 해당 컴파일러 executable 의 절대경로
message(STATUS "C Compiler Abs Path  : ${CMAKE_C_COMPILER}")  

# C 컴파일러의 이름 ex) AppClang, Clang, MSVC, GCC
message(STATUS "C Compiler Name  : ${CMAKE_C_COMPILER_ID}")  
# C 컴파일러의 버전
message(STATUS "C Compiler Version  : ${CMAKE_C_COMPILER_VERSION}")  

# C++ 컴파일러
message(STATUS "C++ Compiler Abs Path : ${CMAKE_CXX_COMPILER}")  
message(STATUS "C++ Compiler Name : ${CMAKE_CXX_COMPILER_ID}")  
message(STATUS "C++ Compiler Version : ${CMAKE_CXX_COMPILER_VERSION}")  

# Build Type 
# - ex) Debug, Release, RelWithDebInfo (디버깅이 가능한 release), MinSizeRel(executable size 최소화))
message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")

# 컴파일러 특징 ex) cxx_std_11, cxx_std_14, cxx_std_17, cxx_std_20
message(STATUS "C++ Compiler Features : ${CMAKE_CXX_COMPILE_FEATURES}")

# Project related
# - "top level" cmakelists.txt 에서 선언된 project() 함수 안에 선언된 이름
message(STATUS "Top Level Project Name : ${CMAKE_PROJECT_NAME}")
message(STATUS "Top Level Project Version : ${CMAKE_PROJECT_VERSION}")
# - 현재 CMakeLists.txt 에서 선언된 project() 함수 안의 이름
message(STATUS "Current Project Name : ${PROJECT_NAME}")
message(STATUS "Current Project Version : ${PROJECT_VERSION}")
# - 현재 CMakeLists.txt 파일이 "top level" 인지 아닌지를 나타내는 flag
message(STATUS "Project Is Top Level : ${PROJECT_IS_TOP_LEVEL}")
message(STATUS "Project Major Version : ${PROJECT_VERSION_MAJOR}")
message(STATUS "Project Minor Version : ${PROJECT_VERSION_MINOR}")
message(STATUS "Project Patch Version : ${PROJECT_VERSION_PATCH}")
message(STATUS "Project Tweak Version : ${PROJECT_VERSION_TWEAK}")
message(STATUS "Project Description : ${PROJECT_DESCRIPTION}")
message(STATUS "Project Homepage Url : ${PROJECT_HOMEPAGE_URL}")

# Project Dir
# - 현재 Local CMake 에서 선언된 project 의 Dir
message(STATUS "Local Project Source Dir : ${PROJECT_SOURCE_DIR}")
message(STATUS "Local Project Binary Dir : ${PROJECT_BINARY_DIR}")
# - Top Level 의 Dir
message(STATUS "Top Level Project Source Dir : ${CMAKE_SOURCE_DIR}")
message(STATUS "Top Level Project Binary Dir : ${CMAKE_BINARY_DIR}")

## -- Variable Definitions -- ##
set(ENGINE_NAME "Engine")
set(EDITOR_NAME "Editor")
set(EXECUTABLE_NAME "Executable")
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

## -- Options -- ##
# can pass variable to set 'COMPILE_EXECUTABLE' value in command line
# ex) cmake .. -DCOMPILE_EXECUTABLE=ON
option(COMPILE_EXECUTABLE "Whether to compile the executable" ON)


## -- include cmake files -- ##
# include(Tools)
include(AddGitSubmodule)

if(ENABLE_LTO)
    # include(LTO)
endif()

if(ENABLE_SANITIZE_ADDR OR ENABLE_SANITIZE_UNDEF)
    # include(Sanitizer)
    # add_sanitizer_flags()
endif()

# 전역 definition
# Set preprocessor definitions to avoid conflicts with minwindef.h (max) versus "std::max"
add_definitions(-DNOMINMAX)

# clang format, cmake format
# add_cmake_format_target()
# add_clang_format_target()

# search for other CMakeLists.txt files in subdirectories
add_subdirectory(external)              # external libraries
add_subdirectory(BJGameEngine)          # Engine
add_subdirectory(BJEditor)              # Editor
add_subdirectory(ApplicationProject)    # Executable
# add_subdirectory(tests)                 # tests

## -- SubModules -- ##
# Do not Use below code since do not want to
# add as sub directory...
# add sub modules cmake files
add_git_submodule(external/Box2D)

# add_git_submodule(external/imgui)
add_git_submodule(external/Glad)
add_git_submodule(external/glm)

# CACHE : set variable as global variable
# INTERNAL : not shown in cmake-gui
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL ON CACHE INTERNAL "Generate installation target")

add_git_submodule(external/GLFW)
# add_git_submodule(external/ImGuizmo)
# add_git_submodule(external/RapidJson)
add_git_submodule(external/spdlog)
add_git_submodule(external/stb_image)

# clang format, cmake format
# add_cmake_format_target()
# add_clang_format_target()



## -- Studies -- ##

### -- 'message' command -- ###
# Set the system version to the latest
# set(CMAKE_SYSTEM_VERSION "latest")
# message(STATUS "Engine CMake System : Windows")

# warning for user
# message(WARNING "Test Warning Message")

# warning for developer
# message(AUTHOR_WARNING "Test Warning Message")

# macro for suppressing developer warnings (ON : above AUTHOR_WARNING not shows)
# set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS OFF CACHE INTERNAL "Suppress Developer Warnings")
    
# error for developer
# configuration still continues but build file is not generated
# message(SEND_ERROR "Test Error Message")

# fatal error for developer
# build , configuration both fails
# message(FATAL_ERROR "Test Fatal Error Message")


### -- 'file system' command -- ###
# CMAKE_SOURCE_DIR : top level cmake file directory
# CMAKE_CURRENT_SOURCE_DIR : current cmake file directory
# CMAKE_BINARY_DIR : built result top level directory ex) /build
# CMAKE_CURRENT_BINARY_DIR : built result 'current'  directory ex) /build/engine
# CMAKE_CURRENT_LIST_DIR : current being executed CMakeLists.txt file directory

# diff between "CMAKE_CURRENT_BINARY_DIR" and "CMAKE_CURRENT_LIST_DIR"
# ex) Hazel/cmake/example.cmake -> include(cmake/example.cmake)
#       즉, 윗단에서 cmake 폴더에 있는 example.cmake를 include하여 실행
#     CMAKE_CURRENT_BINARY_DIR    : /Hazel -> cmake 안에 있는 directory 까지만 출력
#     CMAKE_CURRENT_LIST_DIR        : /Hazel/cmake  -> 전체 경로 모두 출력
#     즉, cmake 를 타고 들어갔을 때 해당 폴더 안에 있는 내용을 찾아야 하는 경우 
#    CMAKE_CURRENT_LIST_DIR를 사용해야 함

# include : including .cmake file ex) similary to #include in C++
# ex) include(example.cmake) : execute example.cmake file 


### -- 'predefined global variable' command -- ###
# >> Platform
# 