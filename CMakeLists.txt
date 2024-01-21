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

# Variable Definitions
set(ENGINE_NAME "Engine")
set(EDITOR_NAME "Editor")
set(EXECUTABLE_NAME "Executable")
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# cmake files
# include(Tools)

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
add_subdirectory(ApplicationProject)    # Application
add_subdirectory(external)              # external libraries
# add_subdirectory(tests)                 # tests
