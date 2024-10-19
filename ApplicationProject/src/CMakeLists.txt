# CONFIGURE_DEPENDS : when file is added or removed, cmake will be reconfigured
# set(ENGINE_SOURCES "my_lib.cc")
file(GLOB EXECUTABLE_SOURCES 
"./*.cpp"
"./*.h"
)
add_executable(
    ${EXECUTABLE_NAME} 
    ${EXECUTABLE_SOURCES})

target_link_libraries(
    ${EXECUTABLE_NAME} 
    PRIVATE
    ${ENGINE_NAME})


# Windows specific settings
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    # Set the system version to the latest
    set(CMAKE_SYSTEM_VERSION "latest")

    # Define Hazel specific preprocessor macros
    target_compile_definitions(
        ${ENGINE_NAME} 
        PUBLIC
        HZ_PLATFORM_WINDOWS
    )

    # Copy the HazelGameEngine DLL to the ApplicationProject directory
    add_custom_command(
        TARGET 
        ${EXECUTABLE_NAME} 
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:${ENGINE_NAME}>
        $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>
    )
endif()



