#include "EditorContext.h"
#include <string>
#include "Hazel/FileSystem/DirectorySystem.h"
#include "Hazel/FileSystem/FileStream.h"
#include "EditorAsset/EditorAssetExtension.h"
#include "Hazel/ImGui/ImGuiContextManager.h"

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
    //assetProcessor = new Editor::LvAssetProcessor();

    _settingsFilePath =
        lv_path_combine(lv_path_application_data(), "EditorSettings.yaml");

    if (!lv_file_exist(_settingsFilePath.c_str()))
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
            Hazel::DirectorySystem::CombinePath(lv_path_application_data(), "EditorSettings.json");
        
        // LvFileStream settingsFile(jsonPath.c_str(), LvFileMode::OPEN);
        Hazel::FileStream settingsFile(jsonPath.c_str(), Hazel::FileOpenMode::OPEN);

        if (settingsFile.GetDataLength() <= 0)
            return;

        LvInputStream inputStream(&settingsFile);

        settingString = inputStream.ReadToEnd();

        settingsFile.Flush();
        settingsFile.Close();

        // lv_file_delete(jsonPath.c_str());
        Hazel::DirectorySystem::DeleteFilePath(jsonPath.c_str());

        if (settingString.empty() || settingString[0] == '{')
        {
            if (!settingString.empty())
            {
                LvJsonDomArchive archive(settingString.c_str());
                settings.Deserialize(archive);
            }

            SaveSettings();
            return;
        }
    }

    LvFileStream settingsFile(_settingsFilePath.c_str(), LvFileMode::OPEN);

    if (settingsFile.Length() <= 0)
        return;

    LvInputStream inputStream(&settingsFile);

    settingString = inputStream.ReadToEnd();

    settingsFile.Flush();
    settingsFile.Close();

    LvYamlArchive archive(settingString.c_str());

    const bool document = settingString[0] == '%';

    if (document)
        archive.ReadStartDocument();
    settings.Deserialize(archive);

    if (document)
        archive.ReadEndDocument();

    if (valueApply)
    {
        SaveSettings();
    }
}
void EditorContext::SaveSettings()
{
    LvString settingString;
    LvYamlArchive archive;

    LvFileStream settingsFile(_settingsFilePath.c_str(), LvFileMode::CREATE);
    LvOutputStream outputStream(&settingsFile);
    archive.WriteStartDocument();
    settings.Serialize(archive);
    archive.WriteEndDocument();
    settingString = archive.GetResult();
    outputStream.WriteText(settingString.c_str());

    settingsFile.Flush();
    settingsFile.Close();
};

EditorContext *BJ_GetEditorContext()
{
    return m_Instance;
}
} // namespace HazelEditor
