#pragma once

#include "Hazel/Core/Object/BaseObject.h"

namespace Hazel
{

//Unity AssetType = https://docs.unity3d.com/Manual/AssetTypes.html
enum class ResourceType : char
{
    IMAGE = 0,
    IMAGE_FONT,
    ANIMATION_GRAPH,
    SCENE_GRAPH,
    PREFAB,
    SCENE,
    SHADER,
    MESH,
    FONT,
    TEXT,
    PARTICLE,
    ANIMATION,
    POSTPROCESS,
    SEQUENCER,
    HLSL,
    UNKNOWN, //폴더 등 (extension없는 것들)
    NOT_ASSET, //Asset이 아닌 것. 엔진에서 쓸 수 없고 예외처리 되는 대상.
    ATLAS,
    SPRITE,
    AUDIO,
    MATERIAL,
    FBX
};

class HAZEL_API ResourceContainer : public BaseObject
{
protected:
    BaseObject *m_Resource;
};
} // namespace Hazel
