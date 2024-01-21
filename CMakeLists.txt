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

# can pass variable to set 'COMPILE_EXECUTABLE' value in command line
# ex) cmake .. -DCOMPILE_EXECUTABLE=ON
option(COMPILE_EXECUTABLE "Whether to compile the executable" ON)

# compiler warning not off !
option(ENABLE_WARNINGS "Enable to add warnings to a target." ON)# always should be on

# if single one warning occurs, compilation will fail
option(ENABLE_WARNINGS_AS_ERRORS "Enable to treat warnings as errors." OFF)

# sanitizer : runtime tool that helps to find memory corruption, memory leak etc.
option(ENABLE_SANITIZE_ADDR "Enable address sanitizer" ON)
option(ENABLE_SANITIZE_UNDEF "Enable undefined sanitizer" ON)

option(ENABLE_LTO "Enable LTO/IPO" ON)

option(ENABLE_CLANG_TIDY "Enable to add clang tidy." ON)

# if not cmake project ? stick to AddGitSubmodule function
# then define own library target
# ex) call function in cmake module file
#     - add_git_submodule(external/json)
option(ENABLE_TESTING "Enable Unit Testing Build" ON)

# Variable Definitions
set(ENGINE_NAME "Engine")
set(EDITOR_NAME "Editor")
set(EXECUTABLE_NAME "Executable")
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

# cmake files
include(Tools)

if(ENABLE_LTO)
    # include(LTO)
endif()

if(ENABLE_SANITIZE_ADDR OR ENABLE_SANITIZE_UNDEF)
    # include(Sanitizer)
    # add_sanitizer_flags()
endif()

# clang format, cmake format
# add_cmake_format_target()
# add_clang_format_target()

# search for other CMakeLists.txt files in subdirectories
add_subdirectory(BJGameEngine)          # Engine
add_subdirectory(BJEditor)          # Editor
add_subdirectory(ApplicationProject)    # Executable
add_subdirectory(external)              # external libraries
add_subdirectory(tests)                 # tests
