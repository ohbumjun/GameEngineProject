# project(imgui)

set(IMGUI_SOURCES
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    # ... other ImGui source files
)

set(IMGUI_HEADERS
    imgui.h
    imgui_internal.h
    # ... other ImGui header files
)
add_library(imgui STATIC
${IMGUI_SOURCES}
${IMGUI_HEADERS})

file(GLOB IMGUI_INCLUDES
"backends/*.h"
)

target_include_directories(imgui
    PUBLIC
    ${IMGUI_INCLUDES}
)
