#pragma once
#ifndef __LV_RENDER_BOUND_COMPONENT_H__
#define __LV_RENDER_BOUND_COMPONENT_H__

#include "engine/LvBound.h"
#include "engine/LvFlag.h"
#include "engine/attribute/LvInspectorAttribute.h"
#include "engine/attribute/LvSerializeVersionAttribute.h"
#include "engine/component/LvComponent.h"

/*#include "engine/component/LvTransformComponent.h" */ namespace Lv
{
namespace Engine
{
class LvTransformComponent;
}
} // namespace Lv

LV_NS_ENGINE_BEGIN

/**
 * @file #include "engine/component/LvRenderBoundComponent.h"
 */
class LV_API LV_REFLECT LV_ATTRIBUTE(LvShowOrderAttribute(9))
    LV_ATTRIBUTE(LvInspectorNameAttribute("Render Bound"))
        LV_ATTRIBUTE(LvHideInInspectorAttribute())
            LV_ATTRIBUTE(LvSerializeVersionAttribute(0)) LvRenderBoundComponent
    : public LvComponent
{
    LV_REFLECTION_FRIEND
public:
    enum class DirtyFlag : uint32
    {
        NONE = 0,
        LOCAL = 1,
    };
    LvDirtyFlag<DirtyFlag> dirtyFlag;
    void SetDirty(uint32 dirtyflag) override
    {
        dirtyflag == LV_UINT_MAX
            ? dirtyFlag.Clear(true)
            : dirtyFlag.Set(static_cast<DirtyFlag>(dirtyflag));
    }

    LvRenderBoundComponent();

    /**
	* @brief local bound 를 반환합니다.
	*/
    LV_FORCEINLINE const LvBoxBound &GetLocalBound() const
    {
        return _localBound;
    }

    LV_FORCEINLINE void SetLocalBound(const LvBoxBound &bound)
    {
        if (_localBound == bound)
            return;
        _localBound = bound;
        dirtyFlag.Add(DirtyFlag::LOCAL);
    }
    /**
	 * @brief Scene::Update에서 Local Bound 기반으로 계산 된 World Bound를 반환하지만, 이는 항상 정확한 값을 보장하지 않습니다.
	 * @details Update 순서에 따라 SetLocalBound의 타이밍이 Update 이후가 되는 경우가 생기는데 이런 경우 World Bound가 최신화 되지 않습니다.
	 */
    LV_FORCEINLINE const LvBoxBound &GetWorldBound() const
    {
        return _worldBound;
    }

    // TODO : move commone update function into system method (after implementing this functions)

    /**
	 * @brief 입력된 Mtx를 localBound에 곱해 worldBound를 업데이트 하는 함수
	 * @param worldMtx localBound를 이동시킬 worldMtx
	 */
    void UpdateWorldBound(const LvMat4f &worldMtx);

    void Serialize(LvArchive &archive) override;

    void Deserialize(LvArchive &archive) override;

protected:
    // Calculate World Bound from Local Bound and then return it
    LvBoxBound calculateWorldBound(const LvVec3f &center,
                                   const LvVec3f &extent,
                                   const LvMat4f &localToWorldMat);


    LV_REFLECT
    LV_ATTRIBUTE(LvInspectorNameAttribute("Local Bound"))
    LvBoxBound _localBound;
    LV_REFLECT
    LV_ATTRIBUTE(LvInspectorNameAttribute("World Bound"))
    LvBoxBound _worldBound;
};

using RenderBoundComponent = LvRenderBoundComponent;

LV_NS_ENGINE_END

#endif