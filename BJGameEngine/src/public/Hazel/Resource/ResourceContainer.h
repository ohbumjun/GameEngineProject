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

/*
Resource ��ü�� ������ Container
ex) Engine -> BundlePack
ex) Editor  -> EditorAsset
*/
class HAZEL_API ResourceContainer : public BaseObject
{
public:
    ResourceContainer(Hazel::ResourceType resourceType,
                      const std::string &resourcePath);

    BaseObject *GetPrototype();

    void SetPrototype(BaseObject* prototype)
    {
		m_Resource = prototype;
    }
    Hazel::ResourceType GetResourceType()
    {
        return m_ResourceType;
    }
    const std::string &GetResourcePath()
    {
        return m_ResourcePath;
    }

protected:
    Hazel::ResourceType m_ResourceType;
    std::string m_ResourcePath;
    BaseObject *m_Resource;
    /**
	*ResourceContainer �� size
	*/
    uint64_t m_Size = 0;
};
} // namespace Hazel
