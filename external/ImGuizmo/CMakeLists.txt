# Add a new static library target for ImGuizmo
add_library(ImGuizmo
    STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ImCurveEdit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ImGradient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ImSequencer.cpp
)

target_include_directories(
    ImGuizmo
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo
    ${CMAKE_SOURCE_DIR}/external/imgui
    )
