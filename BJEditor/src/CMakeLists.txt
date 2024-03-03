# CONFIGURE_DEPENDS : when file is added or removed, cmake will be reconfigured
# set(ENGINE_SOURCES "my_lib.cc")

SET(TOTAL_EDITOR_FILES) # lv1engine 은 그냥 그대로 사용함 ;

SET(FILE_HEADERS 
    public/File/FileManager.h
    public/File/PathManager.h
)
SOURCE_GROUP("file/public" FILES ${FILE_HEADERS})
LIST(APPEND TOTAL_ENGINE_FILES ${FILE_HEADERS})

SET(FILE_SOURCES
    private/File/FileManager.cpp
    private/File/PathManager.cpp
)
SOURCE_GROUP("file/private" FILES ${FILE_SOURCES})
LIST(APPEND TOTAL_ENGINE_FILES ${FILE_SOURCES})

# Create filters for Visual Studio
# foreach(FILE ${EDITOR_SOURCES} ${EDITOR_HEADERS}) 
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
#         set(GROUP "Source Files${GROUP}")
#     elseif("${FILE}" MATCHES ".*\\.h")
#         set(GROUP "Header Files${GROUP}")
#     endif()
# 
#     source_group("${GROUP}" FILES "${FILE}")
# endforeach()

SET(PANEL_HEADERS 
    public/Panel/ComponentPanel/CameraPanel.h
    public/Panel/ContentBrowserPanel.h
    public/Panel/SceneHierarchyPanel.h
)
SOURCE_GROUP("panel/public" FILES ${PANEL_HEADERS})
LIST(APPEND TOTAL_ENGINE_FILES ${PANEL_HEADERS})

SET(PANEL_SOURCES
    private/Panel/ComponentPanel/CameraPanel.cpp
    private/Panel/ContentBrowserPanel.cpp
    private/Panel/SceneHierarchyPanel.cpp
)
SOURCE_GROUP("panel/private" FILES ${PANEL_SOURCES})
LIST(APPEND TOTAL_ENGINE_FILES ${PANEL_SOURCES})


SET(EDITOR_HEADERS
    public/EditorLayer.h
    public/ParticleSystem.h
)
SOURCE_GROUP("panel/private" FILES ${EDITOR_HEADERS})
LIST(APPEND TOTAL_ENGINE_FILES ${EDITOR_HEADERS})


SET(EDITOR_SOURCES
    private/EditorLayer.cpp
    private/ParticleSystem.cpp
    private/HazelEditorApp.cpp
)
SOURCE_GROUP("panel/private" FILES ${EDITOR_SOURCES})
LIST(APPEND TOTAL_ENGINE_FILES ${EDITOR_SOURCES})

set(EDITOR_INCLUDES 
"${CMAKE_SOURCE_DIR}/BJEditor/src/public"
)

# ADD_LIBRARY(${EDITOR_NAME}  SHARED ${TOTAL_EDITOR_FILES})
add_executable(${EDITOR_NAME} 
${TOTAL_ENGINE_FILES}
)

target_compile_definitions(
    ${EDITOR_NAME} 
    PUBLIC 
    # DLL (Dynamic Link Library) toward engine 
    HZ_DYNAMIC_LINK
)

target_include_directories(
    ${EDITOR_NAME} 
    PUBLIC 
    ${EDITOR_INCLUDES})

target_link_libraries(
    ${EDITOR_NAME}
    ${ENGINE_NAME})

# file(
#     GLOB_RECURSE 
#     EDITOR_HEADERS 
#     CONFIGURE_DEPENDS 
# "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
# 
# file(
#     GLOB_RECURSE 
#     EDITOR_SOURCES 
#     CONFIGURE_DEPENDS 
# "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
# 
# set(EDITOR_INCLUDES 
# "${CMAKE_SOURCE_DIR}/BJGameEngine/src"
# "${CMAKE_SOURCE_DIR}/external/spdlog/include"
# "${CMAKE_SOURCE_DIR}/external/glm"
# "${CMAKE_SOURCE_DIR}/external/entt/include"
# "${CMAKE_SOURCE_DIR}/external/RapidJson"
# "${CMAKE_SOURCE_DIR}/external/ImGuizmo"
# )
# 
# add_executable(
#     ${EDITOR_NAME} 
# 
#     ${EDITOR_HEADERS}
#     ${EDITOR_SOURCES}
# )
# 
# target_include_directories(
#     ${EDITOR_NAME} 
#     PUBLIC ${EDITOR_INCLUDES})
# 
# target_link_libraries(
#     ${EDITOR_NAME} 
#     PUBLIC # other libraries
#            # ex) projectname::libraryname (refer to other github cmakelists.txt)
#     ${ENGINE_NAME})

# 아래 두 줄을 통해서 해당 경로에 있는 모든 cpp 파일들을 pch 를 사용하지 않을 것이라고 하는 것이다
# 그러면 hazel engine 은 현재 hzpch.cpp 를 무조건 모든 cpp 파일에서 include 하게 해놨는데
# 이것을 imgui 코드들은 하지 않도록 하는 것이다.
set_source_files_properties(
    ${IMGUIZMO_SOURCES} 
    PROPERTIES SKIP_PRECOMPILE_HEADERS 
    ON
    )

# Set the output directory for the binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY 
${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/${EDITOR_NAME})

# Set the output directory for the intermediate files
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY 
${CMAKE_SOURCE_DIR}/bin-int/${CMAKE_BUILD_TYPE}/${EDITOR_NAME})

# Set platform-specific variables
# Windows specific settings
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    # Set the system version to the latest
    set(CMAKE_SYSTEM_VERSION "latest")

    # Define Hazel specific preprocessor macros
    target_compile_definitions(
        ${EDITOR_NAME} 
        PUBLIC
        HZ_PLATFORM_WINDOWS
    )
endif()


# Debug configuration
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_definitions(${EDITOR_NAME} 
    PUBLIC 
    HZ_DEBUG)

    # game engine을 dll 로 만드는 경우 세팅
    # /MDd for MT runtime library in Debug
    target_compile_options(${EDITOR_NAME} 
    PUBLIC 
    /MDd)  # For MT runtime library

    # DEBUG_POSTFIX: Appends "d" 
    # to the library name in Debug configuration.
    set_target_properties(
        ${EDITOR_NAME} 
    PROPERTIES 
    DEBUG_POSTFIX "d")

    set_target_properties(
        ${EDITOR_NAME} 
    PROPERTIES 
    # RUNTIME_OUTPUT_DIRECTORY_DEBUG ?
    # = Specifies output directory for Debug binaries.
    RUNTIME_OUTPUT_DIRECTORY_DEBUG 
    "${CMAKE_BINARY_DIR}/bin/Debug")

endif()

# Release configuration
if(CMAKE_BUILD_TYPE MATCHES "Release")
    target_compile_definitions(
        ${EDITOR_NAME} 
        PUBLIC 
        HZ_RELEASE)
    set_target_properties(
        ${EDITOR_NAME} 
        PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY_RELEASE 
        "${CMAKE_BINARY_DIR}/bin/Release")
endif()

# Dist configuration
if(CMAKE_BUILD_TYPE MATCHES "Dist")
    target_compile_definitions(
        ${EDITOR_NAME}  
        PUBLIC 
        HZ_DIST)
    set_target_properties(
        ${EDITOR_NAME}  
        PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE 
        "${CMAKE_BINARY_DIR}/bin/Dist")
endif()