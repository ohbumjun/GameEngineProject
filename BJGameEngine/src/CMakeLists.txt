# CONFIGURE_DEPENDS : when file is added or removed, cmake will be reconfigured
# set(ENGINE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/private/**.cpp")
# file(
#     GLOB_RECURSE 
#     ENGINE_HEADERS 
#     CONFIGURE_DEPENDS 
# "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
# 
SET(TOTAL_ENGINE_FILES)

# file(
#     GLOB_RECURSE 
#     ENGINE_SOURCES 
#     "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
# )

SOURCE_GROUP("${CMAKE_CURRENT_SOURCE_DIR}/public" FILES     ${ENGINE_HEADERS})
SOURCE_GROUP("${CMAKE_CURRENT_SOURCE_DIR}/private" FILES    ${ENGINE_SOURCES})

LIST(APPEND TOTAL_ENGINE_FILES ${ENGINE_HEADERS})
LIST(APPEND TOTAL_ENGINE_FILES ${ENGINE_SOURCES})

## Assets
# Asset - Image
SET(SRC_ASSET_IMAGE
    private/Hazel/Asset/Image/SubTexture2D.cpp
    private/Hazel/Asset/Image/Texture.cpp
)
SOURCE_GROUP("asset/image/private" FILES ${SRC_ASSET_IMAGE})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_ASSET_IMAGE})

SET(HEADER_ASSET_IMAGE
    public/Hazel/Asset/Image/SubTexture2D.h
    public/Hazel/Asset/Image/Texture.h
)
SOURCE_GROUP("asset/image/public" FILES ${HEADER_ASSET_IMAGE})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_ASSET_IMAGE})

# Asset - Shader
SET(SRC_ASSET_SHADER
    private/Hazel/Asset/Shader/Shader.cpp
)
SOURCE_GROUP("asset/shader/private" FILES ${SRC_ASSET_SHADER})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_ASSET_SHADER})

SET(HEADER_ASSET_SHADER
    public/Hazel/Asset/Shader/Shader.h
)
SOURCE_GROUP("asset/shader/public" FILES ${HEADER_ASSET_SHADER})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_ASSET_SHADER})

# Asset Codes
SET(HEADER_ASSET
    public/Hazel/Asset/ResourceManager.h
)
SOURCE_GROUP("asset/public" FILES ${HEADER_ASSET})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_ASSET})

SET(SRC_ASSET
    private/Hazel/Asset/ResourceManager.cpp
)
SOURCE_GROUP("asset/private" FILES ${SRC_ASSET})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_ASSET})

## Core
# Core - Allocator
SET(HEADER_CORE_ALLOCATOR
    public/Hazel/Core/Allocation/Allocator/DefaultHeapAllocator.h
    public/Hazel/Core/Allocation/Allocator/FreeListAllocator.h
    public/Hazel/Core/Allocation/Allocator/MemoryPoolAllocator.h
    public/Hazel/Core/Allocation/Allocator/PoolAllocator.h
    public/Hazel/Core/Allocation/Allocator/StackAllocator.h
)
SOURCE_GROUP("core/allocation/allocator/public" FILES ${HEADER_CORE_ALLOCATOR})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_CORE_ALLOCATOR})

SET(SRC_CORE_ALLOCATOR
    private/Hazel/Core/Allocation/Allocator/DefaultHeapAllocator.cpp
    private/Hazel/Core/Allocation/Allocator/FreeListAllocator.cpp
    private/Hazel/Core/Allocation/Allocator/MemoryPoolAllocator.cpp
    private/Hazel/Core/Allocation/Allocator/PoolAllocator.cpp
    private/Hazel/Core/Allocation/Allocator/StackAllocator.cpp
)
SOURCE_GROUP("core/allocation/allocator/private" FILES ${SRC_CORE_ALLOCATOR})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_CORE_ALLOCATOR})

# Core - MemoryPool
SET(HEADER_CORE_MEMORYPOOL
    public/Hazel/Core/Allocation/MemoryPool/BaseMemoryPool.h
    public/Hazel/Core/Allocation/MemoryPool/FixedMemoryPool.h
    public/Hazel/Core/Allocation/MemoryPool/MemoryPoolManager.h
    public/Hazel/Core/Allocation/MemoryPool/VariantMemoryPool.h
)
SOURCE_GROUP("core/allocation/memorypool/public" FILES ${HEADER_CORE_MEMORYPOOL})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_CORE_MEMORYPOOL})

SET(SRC_CORE_MEMORYPOOL
    private/Hazel/Core/Allocation/MemoryPool/BaseMemoryPool.cpp
    private/Hazel/Core/Allocation/MemoryPool/FixedMemoryPool.cpp
    private/Hazel/Core/Allocation/MemoryPool/MemoryPoolManager.cpp
    private/Hazel/Core/Allocation/MemoryPool/VariantMemoryPool.cpp
)
SOURCE_GROUP("core/allocation/memorypool/private" FILES ${SRC_CORE_MEMORYPOOL})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_CORE_MEMORYPOOL})

# Core - GC
SET(HEADER_CORE_GC
    public/Hazel/Core/GC/GC.h
    public/Hazel/Core/GC/GCObject.h
)
SOURCE_GROUP("core/GC/public" FILES ${HEADER_CORE_GC})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_CORE_GC})

SET(SRC_CORE_GC
    private/Hazel/Core/GC/GC.cpp
    private/Hazel/Core/GC/GCObject.cpp
)
SOURCE_GROUP("core/GC/private" FILES ${SRC_CORE_GC})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_CORE_GC})

# Core - ID
SET(HEADER_CORE_ID
    public/Hazel/Core/ID/UUID.h
)
SOURCE_GROUP("core/ID/public" FILES ${HEADER_CORE_ID})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_CORE_ID})

SET(SRC_CORE_ID
    private/Hazel/Core/ID/UUID.cpp
)
SOURCE_GROUP("core/ID/private" FILES ${SRC_CORE_ID})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_CORE_ID})

# Core - Layer
SET(HEADER_CORE_LAYER
    public/Hazel/Core/Layer/Layer.h
    public/Hazel/Core/Layer/LayerStack.h
)
SOURCE_GROUP("core/Layer/public" FILES ${HEADER_CORE_LAYER})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_CORE_LAYER})

SET(SRC_CORE_LAYER
    private/Hazel/Core/Layer/Layer.cpp
    private/Hazel/Core/Layer/LayerStack.cpp
)
SOURCE_GROUP("core/Layer/private" FILES ${SRC_CORE_LAYER})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_CORE_LAYER})

# Core - Object
SET(HEADER_CORE_OBJECT
    public/Hazel/Core/Object/BaseObject.h
    public/Hazel/Core/Object/ObjectDB.h
    public/Hazel/Core/Object/ObjectID.h
    public/Hazel/Core/Object/ObjectPtr.h
)
SOURCE_GROUP("core/Object/public" FILES ${HEADER_CORE_OBJECT})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_CORE_OBJECT})

SET(SRC_CORE_OBJECT
    private/Hazel/Core/Object/BaseObject.cpp
    private/Hazel/Core/Object/ObjectDB.cpp
    private/Hazel/Core/Object/ObjectID.cpp
    private/Hazel/Core/Object/ObjectPtr.cpp
)
SOURCE_GROUP("core/Object/private" FILES ${SRC_CORE_OBJECT})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_CORE_OBJECT})

# Core - Reflection
SET(HEADER_CORE_REFLECTION
    public/Hazel/Core/Reflection/Reflection.h
    public/Hazel/Core/Reflection/TypeUtils.h
    public/Hazel/Core/Reflection/VariableId.h
)
SOURCE_GROUP("core/Reflection/public" FILES ${HEADER_CORE_REFLECTION})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_CORE_REFLECTION})

SET(SRC_CORE_REFLECTION
    private/Hazel/Core/Reflection/Reflection.cpp
    private/Hazel/Core/Reflection/TypeUtils.cpp
    private/Hazel/Core/Reflection/VariableId.cpp
)
SOURCE_GROUP("core/Reflection/private" FILES ${SRC_CORE_REFLECTION})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_CORE_REFLECTION})

# Core - Serialization
SET(HEADER_CORE_SERIALIZATION
    public/Hazel/Core/Serialization/JsonSerializer.h
    public/Hazel/Core/Serialization/Serializer.h
    public/Hazel/Core/Serialization/SerializeTarget.h
)
SOURCE_GROUP("core/Serialization/public" FILES ${HEADER_CORE_SERIALIZATION})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_CORE_SERIALIZATION})

SET(SRC_CORE_SERIALIZATION
    private/Hazel/Core/Serialization/JsonSerializer.cpp
    private/Hazel/Core/Serialization/Serializer.cpp
    private/Hazel/Core/Serialization/SerializeTarget.cpp
)
SOURCE_GROUP("core/Serialization/private" FILES ${SRC_CORE_SERIALIZATION})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_CORE_SERIALIZATION})

# Core - Thread
SET(HEADER_CORE_THREAD
    public/Hazel/Core/Thread/ThreadUtil.h
)
SOURCE_GROUP("core/Thread/public" FILES ${HEADER_CORE_THREAD})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_CORE_THREAD})

SET(SRC_CORE_THREAD
    private/Hazel/Core/Thread/ThreadUtil.cpp
)
SOURCE_GROUP("core/Thread/private" FILES ${SRC_CORE_THREAD})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_CORE_THREAD})

# Core - Application
SET(HEADER_CORE_APPLICATION
    public/Hazel/Core/Application/Application.h
    public/Hazel/Core/Application/Window.h
)
SOURCE_GROUP("core/Application/public" FILES ${HEADER_CORE_APPLICATION})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_CORE_APPLICATION})

SET(SRC_CORE_APPLICATION
    private/Hazel/Core/Application/Application.cpp
    private/Hazel/Core/Application/Window.cpp
)
SOURCE_GROUP("core/Application/private" FILES ${SRC_CORE_APPLICATION})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_CORE_APPLICATION})

# Core - ALL
SET(HEADER_CORE_ALL
    public/Hazel/Core/Core.h
    public/Hazel/Core/EntryPoint.h
)
SOURCE_GROUP("core/public" FILES ${HEADER_CORE_ALL})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_CORE_ALL})

## Debug
SET(HEADER_DEBUG
    public/Hazel/Debug/Instrumentor.h
)
SOURCE_GROUP("debug/public" FILES ${HEADER_DEBUG})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_DEBUG})

SET(SRC_DEBUG
)
SOURCE_GROUP("debug/private" FILES ${SRC_DEBUG})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_DEBUG})

## Event
SET(HEADER_EVENT
    public/Hazel/Event/ApplicationEvent.h
    public/Hazel/Event/Event.h
    public/Hazel/Event/KeyEvent.h
    public/Hazel/Event/MouseEvent.h
)
SOURCE_GROUP("event/public" FILES ${HEADER_EVENT})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_EVENT})

SET(SRC_EVENT
)
SOURCE_GROUP("event/private" FILES ${SRC_EVENT})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_EVENT})

## FileSystem
SET(HEADER_FILESYSTEM
    public/Hazel/FileSystem/DirectorySystem.h
    public/Hazel/FileSystem/FileMemory.h
    public/Hazel/FileSystem/FileModes.h
)
SOURCE_GROUP("filesystem/public" FILES ${HEADER_FILESYSTEM})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_FILESYSTEM})

SET(SRC_FILESYSTEM
    private/Hazel/FileSystem/DirectorySystem.cpp
    private/Hazel/FileSystem/FileMemory.cpp
)
SOURCE_GROUP("filesystem/private" FILES ${SRC_FILESYSTEM})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_FILESYSTEM})

## Imgui
SET(HEADER_IMGUI
    public/Hazel/ImGui/ImGuiLayer.h
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui.h
    ${CMAKE_SOURCE_DIR}/external/imgui/imconfig.h
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_glfw.h
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_opengl3.h
)
SOURCE_GROUP("Imgui/public" FILES ${HEADER_IMGUI})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_IMGUI})

SET(SRC_IMGUI
    private/Hazel/ImGui/ImGuiLayer.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_opengl3.cpp
)
SOURCE_GROUP("Imgui/private" FILES ${SRC_IMGUI})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_IMGUI})

## ImGuizmo
SET(HEADER_IMGUIZMO
    ${CMAKE_SOURCE_DIR}/external/ImGuizmo/ImCurveEdit.h
    ${CMAKE_SOURCE_DIR}/external/ImGuizmo/ImGradient.h
    ${CMAKE_SOURCE_DIR}/external/ImGuizmo/ImGuizmo.h
    ${CMAKE_SOURCE_DIR}/external/ImGuizmo/ImSequencer.h
)
SOURCE_GROUP("ImGuizmo/public" FILES ${HEADER_IMGUIZMO})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_IMGUIZMO})

SET(SRC_IMGUIZMO
    ${CMAKE_SOURCE_DIR}/external/ImGuizmo/ImCurveEdit.cpp
    ${CMAKE_SOURCE_DIR}/external/ImGuizmo/ImGradient.cpp
    ${CMAKE_SOURCE_DIR}/external/ImGuizmo/ImGuizmo.cpp
    ${CMAKE_SOURCE_DIR}/external/ImGuizmo/ImSequencer.cpp
)
SOURCE_GROUP("ImGuizmo/private" FILES ${SRC_IMGUIZMO})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_IMGUIZMO})

## ENTT
SET(SRC_ENTT
    ${CMAKE_SOURCE_DIR}/external/entt/include/entt.hpp
)
SOURCE_GROUP("ImGuizmo/private" FILES ${SRC_ENTT})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_ENTT})

## Input
SET(HEADER_INPUT
    public/Hazel/Input/Input.h
    public/Hazel/Input/KeyCodes.h
    public/Hazel/Input/MouseButtonCodes.h
)
SOURCE_GROUP("input/public" FILES ${HEADER_INPUT})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_INPUT})

SET(SRC_INPUT
)
SOURCE_GROUP("input/private" FILES ${SRC_INPUT})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_INPUT})

## Math
SET(HEADER_MATH
    public/Hazel/Math/Math.h
)
SOURCE_GROUP("math/public" FILES ${HEADER_MATH})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_MATH})

SET(SRC_MATH
    private/Hazel/Math/Math.cpp
)
SOURCE_GROUP("math/private" FILES ${SRC_MATH})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_MATH})

## Component
SET(HEADER_COMPONENT
    public/Hazel/Scene/Component/Collider/BoxCollider2DComponent.h
    public/Hazel/Scene/Component/Collider/CircleCollider2DComponent.h

    public/Hazel/Scene/Component/Identifier/IDComponent.h
    public/Hazel/Scene/Component/Identifier/NameComponent.h

    public/Hazel/Scene/Component/Renderer/CircleRendererComponent.h
    public/Hazel/Scene/Component/Renderer/SpriteRenderComponent.h

    public/Hazel/Scene/Component/CameraComponent.h
    public/Hazel/Scene/Component/Component.h
    public/Hazel/Scene/Component/ComponentFlag.h
    public/Hazel/Scene/Component/NativeScriptComponent.h
    public/Hazel/Scene/Component/RigidBody2DComponent.h
    public/Hazel/Scene/Component/TransformComponent.h
)
SOURCE_GROUP("component/public" FILES ${HEADER_COMPONENT})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_COMPONENT})

SET(SRC_COMPONENT
    private/Hazel/Scene/Component/Collider/BoxCollider2DComponent.cpp
    private/Hazel/Scene/Component/Collider/CircleCollider2DComponent.cpp

    private/Hazel/Scene/Component/Identifier/IDComponent.cpp
    private/Hazel/Scene/Component/Identifier/NameComponent.cpp

    private/Hazel/Scene/Component/Renderer/CircleRendererComponent.cpp
    private/Hazel/Scene/Component/Renderer/SpriteRenderComponent.cpp

    private/Hazel/Scene/Component/CameraComponent.cpp
    private/Hazel/Scene/Component/Component.cpp
    private/Hazel/Scene/Component/NativeScriptComponent.cpp
    private/Hazel/Scene/Component/RigidBody2DComponent.cpp
    private/Hazel/Scene/Component/TransformComponent.cpp
)
SOURCE_GROUP("component/private" FILES ${SRC_COMPONENT})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_COMPONENT})

## Scene 
SET(HEADER_SCENE
    public/Hazel/Scene/Entity.h
    public/Hazel/Scene/Scene.h
    public/Hazel/Scene/SceneCamera.h
    public/Hazel/Scene/SceneSerializer.h
    public/Hazel/Scene/ScriptableEntity.h
)
SOURCE_GROUP("scene/public" FILES ${HEADER_SCENE})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_SCENE})

SET(SRC_SCENE
    private/Hazel/Scene/Entity.cpp
    private/Hazel/Scene/Scene.cpp
    private/Hazel/Scene/SceneCamera.cpp
    private/Hazel/Scene/SceneSerializer.cpp
    private/Hazel/Scene/ScriptableEntity.cpp
)
SOURCE_GROUP("scene/private" FILES ${SRC_SCENE})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_SCENE})

## Utils
SET(HEADER_UTILS
    public/Hazel/Utils/Encoder.h
    public/Hazel/Utils/EngineUtil.h
    public/Hazel/Utils/Log.h
    public/Hazel/Utils/PlatformUtils.h
    public/Hazel/Utils/Random.h
    public/Hazel/Utils/StringUtil.h
    public/Hazel/Utils/TimeStep.h
)
SOURCE_GROUP("utils/public" FILES ${HEADER_UTILS})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_UTILS})

SET(SRC_UTILS
    private/Hazel/Utils/EngineUtil.cpp
    private/Hazel/Utils/Log.cpp
    private/Hazel/Utils/Random.cpp
    private/Hazel/Utils/StringUtil.cpp
)
SOURCE_GROUP("utils/private" FILES ${SRC_UTILS})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_UTILS})

# Platform
## Platform - OpenGL
SET(HEADER_PLATFORM_OPENGL
    public/Platform/OpenGL/OpenGLBuffer.h
    public/Platform/OpenGL/OpenGLContext.h
    public/Platform/OpenGL/OpenGLFrameBuffer.h
    public/Platform/OpenGL/OpenGLRendererAPI.h
    public/Platform/OpenGL/OpenGLShader.h
    public/Platform/OpenGL/OpenGLTexture.h
    public/Platform/OpenGL/OpenGLUniformBuffer.h
    public/Platform/OpenGL/OpenGLVertexArray.h
)
SOURCE_GROUP("platform/opengl/public" FILES ${HEADER_PLATFORM_OPENGL})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_PLATFORM_OPENGL})

SET(SRC_PLATFORM_OPENGL
    private/Platform/OpenGL/OpenGLBuffer.cpp
    private/Platform/OpenGL/OpenGLContext.cpp
    private/Platform/OpenGL/OpenGLFrameBuffer.cpp
    private/Platform/OpenGL/OpenGLRendererAPI.cpp
    private/Platform/OpenGL/OpenGLShader.cpp
    private/Platform/OpenGL/OpenGLTexture.cpp
    private/Platform/OpenGL/OpenGLUniformBuffer.cpp
    private/Platform/OpenGL/OpenGLVertexArray.cpp
)
SOURCE_GROUP("platform/opengl/private" FILES ${SRC_PLATFORM_OPENGL})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_PLATFORM_OPENGL})

## Platform - Windows
SET(HEADER_PLATFORM_WINDOWS
    public/Platform/Windows/WindowPlatformUtil.h
    public/Platform/Windows/WindowsWindow.h
)
SOURCE_GROUP("platform/windows/public" FILES ${HEADER_PLATFORM_WINDOWS})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_PLATFORM_WINDOWS})

SET(SRC_PLATFORM_WINDOWS
    private/Platform/Windows/WindowPlatformUtil.cpp
    private/Platform/Windows/WindowsWindow.cpp
    private/Platform/Windows/WindowsInput.cpp
)
SOURCE_GROUP("platform/windows/private" FILES ${SRC_PLATFORM_WINDOWS})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_PLATFORM_WINDOWS})

# Renderer
## Renderer - Buffer
SET(HEADER_RENDERER_BUFFER
    public/Renderer/Buffer/Buffer.h
    public/Renderer/Buffer/FrameBuffer.h
    public/Renderer/Buffer/UniformBuffer.h
    public/Renderer/Buffer/VertexArray.h
)
SOURCE_GROUP("renderer/buffer/public" FILES ${HEADER_RENDERER_BUFFER})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_RENDERER_BUFFER})

SET(SRC_RENDERER_BUFFER
    private/Renderer/Buffer/Buffer.cpp
    private/Renderer/Buffer/FrameBuffer.cpp
    private/Renderer/Buffer/UniformBuffer.cpp
    private/Renderer/Buffer/VertexArray.cpp
)
SOURCE_GROUP("renderer/buffer/private" FILES ${SRC_RENDERER_BUFFER})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_RENDERER_BUFFER})

## Renderer - Camera
SET(HEADER_RENDERER_CAMERA
    public/Renderer/Camera/Camera.h
    public/Renderer/Camera/EditorCamera.h
    public/Renderer/Camera/OrthographicCamera.h
    public/Renderer/Camera/OrthographicCameraController.h
)
SOURCE_GROUP("renderer/camera/public" FILES ${HEADER_RENDERER_CAMERA})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_RENDERER_CAMERA})

SET(SRC_RENDERER_CAMERA
    private/Renderer/Camera/Camera.cpp
    private/Renderer/Camera/EditorCamera.cpp
    private/Renderer/Camera/OrthographicCamera.cpp
    private/Renderer/Camera/OrthographicCameraController.cpp
)
SOURCE_GROUP("renderer/camera/private" FILES ${SRC_RENDERER_CAMERA})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_RENDERER_CAMERA})

## Renderer
SET(HEADER_RENDERER
    public/Renderer/GraphicsContext.h
    public/Renderer/RenderCommand.h
    public/Renderer/Renderer.h
    public/Renderer/Renderer2D.h
    public/Renderer/RendererAPI.h
)
SOURCE_GROUP("renderer/public" FILES ${HEADER_RENDERER})
LIST(APPEND TOTAL_ENGINE_FILES ${HEADER_RENDERER})

SET(SRC_RENDERER
    private/Renderer/RenderCommand.cpp
    private/Renderer/Renderer.cpp
    private/Renderer/Renderer2D.cpp
    private/Renderer/RendererAPI.cpp
)
SOURCE_GROUP("renderer/public" FILES ${SRC_RENDERER})
LIST(APPEND TOTAL_ENGINE_FILES ${SRC_RENDERER})

# Hazel
SET(HAZEL_HEADER
    Hazel.h
    hzpch.h
    hzpch.cpp
)
SOURCE_GROUP("hazel" FILES ${HAZEL_HEADER})
LIST(APPEND TOTAL_ENGINE_FILES ${HAZEL_HEADER})

# Third Party
SET(THIRDPARTY_SPDLOG)
SOURCE_GROUP("${CMAKE_SOURCE_DIR}/external/spdlog/include" FILES ${THIRDPARTY_SPDLOG})

SET(THIRDPARTY_GLFW)
SOURCE_GROUP("${CMAKE_SOURCE_DIR}/external/GLFW/include" FILES ${THIRDPARTY_GLFW})

SET(THIRDPARTY_GLAD)
SOURCE_GROUP("${CMAKE_SOURCE_DIR}/external/Glad/include" FILES ${THIRDPARTY_GLAD})

SET(THIRDPARTY_IMGUI)
# SOURCE_GROUP("${CMAKE_SOURCE_DIR}/external/imgui" FILES ${THIRDPARTY_IMGUI})
SOURCE_GROUP("${CMAKE_SOURCE_DIR}/external/imgui" FILES ${THIRDPARTY_IMGUI})

SET(THIRDPARTY_GLM)
SOURCE_GROUP("${CMAKE_SOURCE_DIR}/external/glm" FILES ${THIRDPARTY_GLM})

SET(THIRDPARTY_STBIMAGE)
SOURCE_GROUP("${CMAKE_SOURCE_DIR}/external/stb_image" FILES ${THIRDPARTY_STBIMAGE})

SET(THIRDPARTY_ENTT)
SOURCE_GROUP("${CMAKE_SOURCE_DIR}/external/entt" FILES ${THIRDPARTY_ENTT})

SET(THIRDPARTY_RAPIDJSON)
SOURCE_GROUP("${CMAKE_SOURCE_DIR}/external/RapidJson" FILES ${THIRDPARTY_RAPIDJSON})

SET(THIRDPARTY_IMGUIZMO)
SOURCE_GROUP("${CMAKE_SOURCE_DIR}/external/ImGuizmo" FILES ${THIRDPARTY_IMGUIZMO})

SET(THIRDPARTY_BOX2D)
SOURCE_GROUP("${CMAKE_SOURCE_DIR}/external/Box2D/include" FILES ${THIRDPARTY_BOX2D})


# is this lib file ?
# Set the output directory for the binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY 
${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/${ENGINE_NAME})

# is this obj file ?
# Set the output directory for the intermediate files
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY 
${CMAKE_SOURCE_DIR}/bin-int/${CMAKE_BUILD_TYPE}/${ENGINE_NAME})

set(ENGINE_INCLUDES
   "${CMAKE_SOURCE_DIR}/BJGameEngine/src"
   "${CMAKE_SOURCE_DIR}/external/spdlog/include"
   "${CMAKE_SOURCE_DIR}/external/GLFW/include"
   "${CMAKE_SOURCE_DIR}/external/Glad/include"
   "${CMAKE_SOURCE_DIR}/external/imgui"
   "${CMAKE_SOURCE_DIR}/external/glm"
   "${CMAKE_SOURCE_DIR}/external/stb_image"
   "${CMAKE_SOURCE_DIR}/external/entt/include"
   "${CMAKE_SOURCE_DIR}/external/RapidJson"
   "${CMAKE_SOURCE_DIR}/external/ImGuizmo"
   "${CMAKE_SOURCE_DIR}/external/Box2D/include"
   "${CMAKE_CURRENT_SOURCE_DIR}/public" 
   )
   
add_library(
    ${ENGINE_NAME}
    SHARED 
    ${TOTAL_ENGINE_FILES}
    ${THIRDPARTY_SPDLOG}
    ${THIRDPARTY_GLFW}
    ${THIRDPARTY_GLAD}
    ${THIRDPARTY_IMGUI}
    ${THIRDPARTY_GLM}
    ${THIRDPARTY_STBIMAGE}
    ${THIRDPARTY_ENTT}
    ${THIRDPARTY_RAPIDJSON}
    ${THIRDPARTY_IMGUIZMO}
    ${THIRDPARTY_BOX2D}

    # 여기에 다른 폴더들도 넣어줘야 하는가 ?
)


# Add precompiled header source file
target_precompile_headers(
    ${ENGINE_NAME} 
    PUBLIC 
    # "${CMAKE_CURRENT_SOURCE_DIR}/HazelGameEngine/src/hzpch.h"
    "hzpch.h"
)

target_compile_definitions(
    ${ENGINE_NAME} 
    PUBLIC 
    HZ_ENABLE_ASSERTS
    _CRT_SECURE_NO_WARNINGS 
    GLFW_INCLUDE_NONE
    _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING

    # DLL
    HZ_DYNAMIC_LINK
    HZ_BUILD_DLL
    )
# where to find header files
# cpp file can include header files from "./~~"
target_include_directories(
    ${ENGINE_NAME} 
    PUBLIC 
    ${ENGINE_INCLUDES})
# Create filters for Visual Studio
# foreach(FILE ${ENGINE_SOURCES} ${ENGINE_HEADERS}) 
#     # Get the directory of the source file
#     get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
# 
#     # Remove common directory prefix to make the group
#     string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
# 
#     # Make sure we are using windows slashes
#     string(REPLACE "/" "\\" GROUP "${GROUP}")
# 
#     # Group into "Source Files" and "Header Files"
#     if ("${FILE}" MATCHES ".*\\.cpp")
#        set(GROUP "Source Files${GROUP}")
#     elseif("${FILE}" MATCHES ".*\\.h")
#        set(GROUP "Header Files${GROUP}")
#     endif()
# 
#     source_group("${GROUP}" FILES "${FILE}")
# endforeach()

# create target, but not executable == library
# STATIC : staic library    ex) .lib
# SHARED : dynamic library  ex) .dll
# add_library(
#     ${ENGINE_NAME} 
#     SHARED 
#     ${TOTAL_ENGINE_FILES}
#     ${ENGINE_SOURCES}
#     ${ENGINE_INCLUDES}
#     )



# link library to own library
target_link_libraries(
    ${ENGINE_NAME}
    PUBLIC # other libraries
           # ex) projectname::libraryname (refer to other github cmakelists.txt)
    box2d  # find project name in actual cmakefile
    # imgui
    # ImGuizmo
    Glad
    glm
    opengl32.lib
    stb_image
    glfw
    spdlog
    )

# 아래 두 줄을 통해서 해당 경로에 있는 모든 cpp 파일들을 pch 를 사용하지 않을 것이라고 하는 것이다
# 그러면 hazel engine 은 현재 hzpch.cpp 를 무조건 모든 cpp 파일에서 include 하게 해놨는데
# 이것을 imgui 코드들은 하지 않도록 하는 것이다.
set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/external/ImGuizmo/**.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stb_image/**.cpp
    PROPERTIES COMPILE_FLAGS "/Yc;pch.h"
)

# Windows specific settings
if(CMAKE_SYSTEM_NAME MATCHES "Windows")

    # Set the system version to the latest
    set(CMAKE_SYSTEM_VERSION "latest")
    message(STATUS "Engine CMake System : Windows")

    # warning for user
    message(WARNING "Test Warning Message")

    # warning for developer
    # message(AUTHOR_WARNING "Test Warning Message")

    # macro for suppressing developer warnings (ON : above AUTHOR_WARNING not shows)
    # set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS OFF CACHE INTERNAL "Suppress Developer Warnings")
    
    # error for developer
    # configuration still continues but build file is not generated
    # message(SEND_ERROR "Test Error Message")

    # Define Hazel specific preprocessor macros
    target_compile_definitions(
        ${ENGINE_NAME} 
        PUBLIC
        HZ_PLATFORM_WINDOWS
    )
    
    # visibility of symbols ex) function, class, variable
    # 'on' : all symbols are exported and visibile to other libraries, exes
    set_target_properties( ${ENGINE_NAME} 
        PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS 
    ON )
endif()


# Debug configuration
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_definitions(${ENGINE_NAME} 
    PUBLIC 
    HZ_DEBUG)

    # game engine을 dll 로 만드는 경우 세팅
    # /MDd for MT runtime library in Debug
    target_compile_options(
        ${ENGINE_NAME} 
    PUBLIC 
    # For MT runtime library
    /MDd)  

    # DEBUG_POSTFIX: Appends "d" 
    # to the library name in Debug configuration.
    set_target_properties(
        ${ENGINE_NAME} 
    PROPERTIES 
    DEBUG_POSTFIX "d")

    set_target_properties(
        ${ENGINE_NAME} 
    PROPERTIES 
    # RUNTIME_OUTPUT_DIRECTORY_DEBUG ?
    # = Specifies output directory for Debug binaries.
    RUNTIME_OUTPUT_DIRECTORY_DEBUG 
    "${CMAKE_BINARY_DIR}/bin/Debug")

endif()

# Release configuration
if(CMAKE_BUILD_TYPE MATCHES "Release")
    target_compile_definitions(
        ${ENGINE_NAME} 
        PUBLIC 
        HZ_RELEASE)
    set_target_properties(
        ${ENGINE_NAME} 
        PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY_RELEASE 
        "${CMAKE_BINARY_DIR}/bin/Release")
endif()

# Dist configuration
if(CMAKE_BUILD_TYPE MATCHES "Dist")
    target_compile_definitions(
        ${ENGINE_NAME}  
        PUBLIC 
        HZ_DIST)
    set_target_properties(
        ${ENGINE_NAME}  
        PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE 
        "${CMAKE_BINARY_DIR}/bin/Dist")
endif()