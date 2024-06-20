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
    UNKNOWN, //���� �� (extension���� �͵�)
    NOT_ASSET, //Asset�� �ƴ� ��. �������� �� �� ���� ����ó�� �Ǵ� ���.
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
