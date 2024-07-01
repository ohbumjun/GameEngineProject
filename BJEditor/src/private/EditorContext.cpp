#include "EditorContext.h"
#include <string>
#include "Hazel/FileSystem/DirectorySystem.h"
#include "Hazel/FileSystem/FileStream.h"
#include "EditorAsset/EditorAssetExtension.h"
#include "Hazel/Core/Serialization/JsonSerializer.h"
#include "Hazel/ImGui/ImGuiContextManager.h"
#include "Hazel/Core/Application/Application.h"

namespace HazelEditor
{
const char *EditorContext::Directories::engine = "Engine";
const char *EditorContext::Directories::bundle = "Bundle";
const char *EditorContext::Directories::settings = "Settings";
const char *EditorContext::Directories::solution = "Solution";
const char *EditorContext::Directories::library = "Library";
const char *EditorContext::Directories::project = "Project";
const char *EditorContext::Directories::resources = "Resources";
const char *EditorContext::Directories::builtin = "BuiltIn";
// const char *EditorContext::Directories::builtinresource = stringBuiltIn.c_str();
const char *EditorContext::Directories::program = "Program";

const char *EditorContext::Directories::Cache::asset = "AssetCache";
const char *EditorContext::Directories::Cache::reflect = "ReflectCache";
const char *EditorContext::Directories::Cache::shader = "ShaderCache";
const char *EditorContext::Directories::Cache::builtInShader = "BuiltInShaderCache";

EditorContext *s_EditorContextInstance = nullptr;

std::string s_EditorRootPath;

Editor *EditorContext::Initialize()
{
    const std::string& engineRootPath = Hazel::Application::Get().GetSpecification().GetEngineRootPath();

    s_EditorRootPath =
        Hazel::DirectorySystem::CombinePath(engineRootPath.c_str(), "Editor");

    s_EditorContextInstance = new EditorContext();

    // AssetExtension
    EditorAssetExtension::Initialize();

    // ImguiContextManager
    Hazel::ImguiContextManager::Initialize();
    Hazel::ImguiContextManager::CreateContext();

    // Hazel::AssetManagerController::Initialize(
    //     new HazelEditor::EditorAssetManagerController());

    // CoreCLR

    // Editor 생성
    return new Editor();
}

EditorContext::EditorContext()
{
    //assetProcessor = new Editor::LvAssetProcessor();

    // BJEditor/EditosSettings.json 와 같은 파일 경로가 되어야 한다. 
    _settingsFilePath =
        // lv_path_combine(lv_path_application_data(), "EditorSettings.yaml");
        Hazel::DirectorySystem::CombinePath(s_EditorRootPath.c_str(),
                                            "EditorSettings.json");

    // if (!lv_file_exist(_settingsFilePath.c_str()))
    if (!Hazel::DirectorySystem::ExistFilePath(_settingsFilePath.c_str()))
    {
        SaveSettings();
    }
    else
    {
        LoadSettings();
    }
}

EditorContext::~EditorContext()
{
}
void EditorContext::LoadSettings()
{
    std::string settingString;

    if (!Hazel::DirectorySystem::ExistFilePath(_settingsFilePath.c_str()))
    {
        const std::string jsonPath =
            // lv_path_combine(lv_path_application_data(), "EditorSettings.json");
            Hazel::DirectorySystem::CombinePath(s_EditorRootPath.c_str(),
                                                "EditorSettings.json");
        
        // LvFileStream settingsFile(jsonPath.c_str(), LvFileMode::OPEN);
        Hazel::FileStream settingsFile(jsonPath.c_str(), Hazel::FileOpenMode::OPEN);

        if (settingsFile.GetDataLength() <= 0)
            return;

        Hazel::InputStream inputStream(&settingsFile);

        settingString = inputStream.ReadToEnd();

        settingsFile.FlushToFile();
        settingsFile.End();

        // lv_file_delete(jsonPath.c_str());
        Hazel::DirectorySystem::DeleteFilePath(jsonPath.c_str());

        if (settingString.empty() || settingString[0] == '{')
        {
            if (!settingString.empty())
            {
               //  LvJsonDomArchive archive(settingString.c_str());
                Hazel::JsonSerializer archive(settingString.c_str());
               m_Settings.Deserialize(&archive);
            }

            SaveSettings();
            return;
        }
    }

    Hazel::FileStream settingsFile(_settingsFilePath.c_str(), Hazel::FileOpenMode::OPEN);

    if (settingsFile.GetDataLength() <= 0)
        return;

    Hazel::InputStream inputStream(&settingsFile);

    settingString = inputStream.ReadToEnd();

    settingsFile.FlushToFile();
    settingsFile.End();

    // LvYamlArchive archive(settingString.c_str());
    // 
    // const bool document = settingString[0] == '%';
    // 
    // if (document)
    //     archive.ReadStartDocument();
    // settings.Deserialize(archive);
    // 
    // if (document)
    //     archive.ReadEndDocument();
    // 
    // if (valueApply)
    // {
        SaveSettings();
    // }
}
void EditorContext::SaveSettings()
{
    std::string settingString;
    // LvYamlArchive archive;
    Hazel::JsonSerializer archive;

    Hazel::FileStream settingsFile(_settingsFilePath.c_str(), Hazel::FileOpenMode::CREATE);
    Hazel::OutputStream outputStream(&settingsFile);

    archive.BeginSaveMap();
    m_Settings.Serialize(&archive);
    archive.EndSaveMap();

    settingString = archive.GetFinalResult();
    outputStream.WriteText(settingString.c_str());

    settingsFile.FlushToFile();
    settingsFile.End();
};

EditorContext *BJ_GetEditorContext()
{
    return s_EditorContextInstance;
}
const char *BJ_GetEditorDirRootPath()
{
    return s_EditorRootPath.c_str();
}
} // namespace HazelEditor
