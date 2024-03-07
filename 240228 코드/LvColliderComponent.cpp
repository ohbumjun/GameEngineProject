#include "engine/component/LvColliderComponent.h"
#include "LvPrecompiled.h"

#include "system/LvReflection.h"
#include "system/math/LvMat3f.h"

#include "engine/LvObject.h"
#include "engine/attribute/LvComponentAttribute.h"
#include "engine/component/LvTransformComponent.h"


using namespace Lv;


LV_NS_ENGINE_BEGIN

LvColliderComponent::LvColliderComponent()
{
    _localBound = LvBoxBound(LvVec3f(0.f, 0.f, 0.f), 1.f);
    _worldBound = LvBoxBound();
}

LvBoxBound LvColliderComponent::calculateWorldBound(
    const LvVec3f &center,
    const LvVec3f &extent,
    const LvMat4f &localToWorldMat)
{
    // AABB update with transform
    /*
	* // Code fetched from Unreal Engine
	_worldBound = _localBound;
	const LvMat4f trans = transform.localToWorld;
	const LvVec3f worldBoundCenter = _worldBound.GetCenter();
	const LvSimd128f VecOrigin = lv_simd_load_float3(&worldBoundCenter);
	const LvSimd128f VecExtent = lv_simd_load_float3(&_worldBound.extent.x);

	const LvSimd128f m0 = lv_simd_make(trans[0][0], trans[1][0], trans[2][0], 0.f);
	const LvSimd128f m1 = lv_simd_make(trans[0][1], trans[1][1], trans[2][1], 0.f);
	const LvSimd128f m2 = lv_simd_make(trans[0][2], trans[1][2], trans[2][2], 0.f);
	const LvSimd128f m3 = lv_simd_load_align(&(trans.cols[3].x));

	LvSimd128f NewOrigin = lv_simd_mul(lv_simd_replicate(VecOrigin, 0), m0);
	NewOrigin = lv_simd_mul_add(lv_simd_replicate(VecOrigin, 1), m1, NewOrigin);
	NewOrigin = lv_simd_mul_add(lv_simd_replicate(VecOrigin, 2), m2, NewOrigin);
	NewOrigin = lv_simd_add(NewOrigin, m3);

	LvSimd128f NewExtent = lv_simd_abs(lv_simd_mul(lv_simd_replicate(VecExtent, 0), m0));
	NewExtent = lv_simd_add(NewExtent, lv_simd_abs(lv_simd_mul(lv_simd_replicate(VecExtent, 1), m1)));
	NewExtent = lv_simd_add(NewExtent, lv_simd_abs(lv_simd_mul(lv_simd_replicate(VecExtent, 2), m2)));

	LvVec3f calculatedOrigin;
	lv_simd_store_float3(NewExtent, &_worldBound.extent.x);
	lv_simd_store_float3(NewOrigin, &calculatedOrigin);
	_worldBound.minp = calculatedOrigin - _worldBound.extent;
	_worldBound.maxp = calculatedOrigin + _worldBound.extent;

	// Sphere update with transform
	LvSimd128f MaxRadius = lv_simd_mul(m0, m0);
	MaxRadius = lv_simd_mul_add(m1, m1, MaxRadius);
	MaxRadius = lv_simd_mul_add(m2, m2, MaxRadius);
	MaxRadius = lv_simd_max(lv_simd_max(MaxRadius, lv_simd_replicate(MaxRadius, 1)), lv_simd_replicate(MaxRadius, 2));
	_worldBound.radius = LvMath::Sqrtf(lv_simd_get_component(MaxRadius, 0)) * _worldBound.radius;

	// For non-uniform scaling, computing sphere radius from a box results in a smaller sphere.
	const float BoxExtentMagnitude = LvMath::Sqrtf(lv_simd_get_component(lv_simd_dot3(NewExtent, NewExtent), 0));
	_worldBound.radius = LV_MIN(_worldBound.radius, BoxExtentMagnitude);
	*/

    LvBoxBound worldBound;
    LvMat3f absRotate;
    for (uint i = 0; i < 3; ++i)
    {
        for (uint j = 0; j < 3; ++j)
        {
            absRotate[i][j] = LvMath::Abs(localToWorldMat[j][i]);
        }
    }

    for (uint i = 0; i < 3; ++i)
    {
        worldBound.extent[i] = LvVec3f::Dot(absRotate[i], extent);
    }

    LvVec3f newCenter = localToWorldMat * LvVec4f(center, 1.f);

    worldBound.minp = newCenter - worldBound.extent;
    worldBound.maxp = newCenter + worldBound.extent;
    worldBound.radius = LvVec3f::Length(worldBound.extent) * 0.5f;

    return worldBound;
}

void LvColliderComponent::UpdateWorldBound(const LvMat4f &worldMtx)
{
    _worldBound = calculateWorldBound(_localBound.GetCenter(),
                                      _localBound.extent,
                                      worldMtx);
}

void LvColliderComponent::Serialize(LvArchive &archive)
{
    archive.WriteStartObject(_type, this);
    LV_WRITE(archive, enabled);
    LV_WRITE(archive, _localBound);
    archive.WriteEndObject();
}

void LvColliderComponent::Deserialize(LvArchive &archive)
{
    archive.ReadStartObject(_type, this);
    LV_READ(archive, enabled);
    LV_READ(archive, _localBound);
    archive.ReadEndObject();
}

LV_NS_ENGINE_END