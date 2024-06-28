#include "hzpch.h"
#include "EditorContext.h"
#include "Editor.h"
#include "EditorAsset/EditorAssetExtension.h"
#include "Hazel/Resource/AssetManagerBase.h"
#include "EditorAsset/EditorAssetManager.h"
#include "Hazel/ImGui/ImGuiContextManager.h"
#include "Hazel/Core/Application/Application.h"
#include "File/FileManager.h"

namespace HazelEditor
{

EditorContext *EditorContext::m_Instance = nullptr;

Editor *EditorContext::Initialize()
{
    m_Instance = new EditorContext();

    // AssetExtension
    EditorAssetExtension::Initialize();

    Hazel::AssetManagerController::Initialize(
        new HazelEditor::EditorAssetManagerController());

    // ImguiContextManager
    Hazel::ImguiContextManager::Initialize();
    ImGuiContext *context = Hazel::ImguiContextManager::CreateContext();

    const Hazel::ApplicationContext &applicationContext =
        Hazel::Application::Get().GetSpecification();
    const Hazel::ApplicationCommandLineArgs &applicationCommandLineArgs =
        applicationContext.GetCommandLineArgs();

    HazelEditor::FileManager::Initialize(applicationContext.ge);

    // CoreCLR

    // Editor »ý¼º
    return new Editor();
}

EditorContext::EditorContext()
{
}

EditorContext::~EditorContext(){};

};

