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

## -- Subdirectories -- ##
# search for other CMakeLists.txt files in subdirectories
add_subdirectory(BJGameEngine)          # Engine
# add_subdirectory(BJEditor)  
add_subdirectory(ApplicationProject)    # Application
add_subdirectory(external)              # external libraries
# add_subdirectory(tests)                 # tests

## -- SubModules -- ##
# Do not Use below code since do not want to
# add as sub directory...
# add sub modules cmake files
add_git_submodule(external/Box2D)
add_git_submodule(external/imgui)
add_git_submodule(external/Glad)
add_git_submodule(external/glm)

set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

add_git_submodule(external/GLFW)
add_git_submodule(external/ImGuizmo)
# add_git_submodule(external/RapidJson)
add_git_submodule(external/spdlog)
add_git_submodule(external/stb_image)

# clang format, cmake format
# add_cmake_format_target()
# add_clang_format_target()
