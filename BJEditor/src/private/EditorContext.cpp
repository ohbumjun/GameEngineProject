#include "EditorContext.h"
#include <string>
#include "Hazel/FileSystem/DirectorySystem.h"
#include "EditorAsset/EditorAssetExtension.h"
#include "Hazel/ImGui/ImGuiContextManager.h"

namespace HazelEditor
{
const char *EditorContext::Directories::engine = "Engine";
const char *EditorContext::Directories::bundle = "Bundle";
const char *EditorContext::Directories::settings = "Settings";
const char *EditorContext::Directories::solution = "Solution";
const char *EditorContext::Directories::library = "Library";
const char *EditorContext::Directories::log = "Log";
const char *EditorContext::Directories::project = "Project";
const char *EditorContext::Directories::resources = "Resources";
const char *EditorContext::Directories::builtin = "BuiltIn";
// const char *EditorContext::Directories::builtinresource = stringBuiltIn.c_str();
const char *EditorContext::Directories::program = "Program";


EditorContext *EditorContext::m_Instance = nullptr;

Editor *EditorContext::Initialize()
{
    m_Instance = new EditorContext();

    // AssetExtension
    EditorAssetExtension::Initialize();

    // ImguiContextManager
    Hazel::ImguiContextManager::Initialize();
    Hazel::ImguiContextManager::CreateContext();

    // Hazel::AssetManagerController::Initialize(
    //     new HazelEditor::EditorAssetManagerController());

    // CoreCLR

    // Editor »ý¼º
    return new Editor();
}

EditorContext::EditorContext()
{
}

EditorContext::~EditorContext(){};
} // namespace HazelEditor
