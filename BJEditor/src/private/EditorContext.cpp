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

    // Editor 생성
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
    LvString settingString;

    // @donghun @TODO 해당 예외처리는 나중에 마이너 버전이 올라가면 제거할 것.
    if (!lv_file_exist(_settingsFilePath.c_str()))
    {
        const LvString jsonPath =
            lv_path_combine(lv_path_application_data(), "EditorSettings.json");
        LvFileStream settingsFile(jsonPath.c_str(), LvFileMode::OPEN);

        if (settingsFile.Length() <= 0)
            return;

        LvInputStream inputStream(&settingsFile);

        settingString = inputStream.ReadToEnd();

        settingsFile.Flush();
        settingsFile.Close();

        lv_file_delete(jsonPath.c_str());

        if (settingString.IsEmpty() || settingString[0] == '{')
        {
            if (!settingString.IsEmpty())
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

    // @donggun 잘못된 값을 로드 하였다면 수정 하여야 함.
    bool valueApply = false;
    if (settings.outLogFilePath.IsEmpty() ||
        !lv_file_exist(settings.outLogFilePath.c_str()))
    {
        settings.outLogFilePath =
            lv_path_combine(lv_path_application_data(), "editor.log");
        valueApply = true;
    }

    if (settings.autoCreateProject.IsEmpty() ||
        !lv_directory_exist(settings.autoCreateProject.c_str()))
    {
        settings.autoCreateProject =
            lv_path_combine(lv_path_application_data(), "Projects");
        valueApply = true;
    }

    if (0 == settings.logMaxCount)
    {
        settings.logMaxCount = LvEditorSettings().logMaxCount;
        valueApply = true;
    }

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
