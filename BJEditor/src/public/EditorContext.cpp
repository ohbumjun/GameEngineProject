#include "EditorContext.h"
#include "Editor.h"
#include "EditorAsset/EditorAssetExtension.h"

namespace HazelEditor
{

EditorContext *EditorContext::m_Instance = nullptr;

Editor *EditorContext::Initialize()
{
    m_Instance = new EditorContext();

    // AssetExtension
    EditorAssetExtension::Initialize();

    // CoreCLR

    // Editor »ý¼º
    return new Editor();
}

EditorContext::EditorContext()
{
}

EditorContext::~EditorContext()
{
}

}

