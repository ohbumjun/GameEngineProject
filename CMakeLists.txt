# set(CMAKE_CONFIGURATION_TYPES "Debug;RelWithDebInfo" CACHE STRING "" FORCE)
cmake_minimum_required(VERSION 3.22)

project(
    BumJunEngine
    VERSION 1.0.0
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

# ex) some c variable not in C++ standard. but can be used in gcc
#     disable these kinds of examples
set(CMAKE_CXX_EXTENSIONS OFF)

# generate 'compile_commands.json' file so that clang-tidy can know
# which files to look for
# ex) clang-tidy is a tool that runs before compilation
#     so it has to know where to look for all files of project
#     based on compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

## -- Useful Predifined Variables -- ##
# OS Name ex) Darwin(MacOS), Linux, Windows
message(STATUS "CMAKE_SYSTEM_NAME : ${CMAKE_SYSTEM_NAME}")

# OS Version
message(STATUS "CMAKE_SYSTEM_VERSION : ${CMAKE_SYSTEM_VERSION}") 

# OS Name - OS Version
message(STATUS "CMAKE_SYSTEM : ${CMAKE_SYSTEM}")  

# CPU-Architecture ex) x86, x86_64, arm, arm64
message(STATUS "CMAKE_SYSTEM_PROCESSOR : ${CMAKE_SYSTEM_PROCESSOR}")  

# CMake 를 "run" 하는 시스템의 이름
# ex) Cross -Compiling 을 할 때, Build 시스템과 Run 시스템이 다를 수 있다.
# ex) CMAKE_SYSTEM 은 "Build" 시스템의 이름을 나타낸다.
# message(STATUS "CMAKE_HOST_SYSTEM               : ${CMAKE_HOST_SYSTEM}")  
# message(STATUS "CMAKE_HOST_SYSTEM_NAME      : ${CMAKE_HOST_SYSTEM_NAME}")  
# message(STATUS "CMAKE_HOST_SYSTEM_VERSION : ${CMAKE_HOST_SYSTEM_VERSION}")  
# message(STATUS "CMAKE_HOST_SYSTEM_PROCESSOR : ${CMAKE_HOST_SYSTEM_PROCESSOR}")  

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