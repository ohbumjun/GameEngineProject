# CONFIGURE_DEPENDS : when file is added or removed, cmake will be reconfigured
# set(ENGINE_SOURCES "my_lib.cc")
file(GLOB EXECUTABLE_SOURCES 
"./*.cpp"
"./*.h"
)
add_executable(
    ${EXECUTABLE_NAME} ${EXECUTABLE_SOURCES})

target_link_libraries(
    ${EXECUTABLE_NAME} 
    ${ENGINE_NAME})



