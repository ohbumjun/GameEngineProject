# Add a new static library target for ImGuizmo
# add_library(ImGuizmo
#    STATIC
#    ${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/ImCurveEdit.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/ImGradient.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/ImSequencer.cpp
#)

set(
   IMGUIZMO_HEADERS
   "${CMAKE_CURRENT_SOURCE_DIR}/ImSequencer.h"
   "${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo.h"
   "${CMAKE_CURRENT_SOURCE_DIR}/ImGradient.h"
   "${CMAKE_CURRENT_SOURCE_DIR}/ImCurveEdit.h"
)

set(
   IMGUIZMO_SOURCES
   "${CMAKE_CURRENT_SOURCE_DIR}/ImSequencer.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/ImGradient.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/ImCurveEdit.cpp"
)

add_library(
   ImGuizmo
    SHARED
    ${IMGUIZMO_HEADERS}
    ${IMGUIZMO_SOURCES}
)

target_include_directories(
    ImGuizmo
    PUBLIC
    ${CMAKE_SOURCE_DIR}/external/ImGuizmo/
    ${CMAKE_SOURCE_DIR}/external/imgui
#     # "${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_glfw.h"
#     # "${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_opengl3.h"
)

target_link_libraries(
   ImGuizmo
    PUBLIC # other libraries
           # ex) projectname::libraryname (refer to other github cmakelists.txt)
    imgui
)
