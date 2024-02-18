
add_library(
    stb_image
    STATIC 
     "${CMAKE_CURRENT_SOURCE_DIR}/stb_image.cpp")

target_include_directories(
    stb_image 
    PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/stb_image.h")
