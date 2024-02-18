add_library(
    Glad 
    STATIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c")

target_include_directories(
    Glad
    PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include")

