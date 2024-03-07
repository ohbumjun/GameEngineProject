#pragma once
#ifndef __LV_BOUND_H__
#define __LV_BOUND_H__

#include "LvPrecompiled.h"
#include "engine/LvGeometry.h"
#include "engine/archive/LvArchivable.h"
#include "system/math/LvMatSimdf.h"
#include "system/math/LvMath.h"
#include "system/math/LvQuatf.h"
#include "system/math/LvVec2f.h"
#include "system/math/LvVec3f.h"
#include "system/math/LvVec4f.h"
#include "system/math/LvVecSimdf.h"

/* #include "engine/LvMesh.h" */ namespace Lv
{
namespace Engine
{
class LvMesh;
}
} // namespace Lv

LV_NS_ENGINE_BEGIN

enum class LvBoundType : uint8
{
    BOX,
    ORIENTEDBOX,
    MESH,
};

/**
 * @file #include "engine/LvBound.h"
 */
class LV_API LV_REFLECT LvBound : public LvArchivable
{
public:
    LvBound(LvBoundType type = LvBoundType::BOX) : type(type)
    {
    }

    LvBoundType type;

    virtual LV_FORCEINLINE bool IsValid() noexcept = 0;
    virtual LV_FORCEINLINE void Clear() noexcept = 0;

    virtual void ExpandToInclude(const LvVec3f &p) = 0;
    virtual void ExpandToInclude(const LvBound &bound) = 0;

    virtual bool Intersect(const LvRay &ray) const = 0;
    virtual bool Intersect(const LvRay &ray, float &tNear) const = 0;
    virtual bool Intersect(const LvRay &ray,
                           float &tNear,
                           float &tFar) const = 0;

    virtual void Serialize(LvArchive &archive)
    {
    }
    virtual void Deserialize(LvArchive &archive)
    {
    }
};

using Bound = LvBound;

class LV_API LV_REFLECT LvBoxBound : public LvBound
{
public:
    //boundary box's point near to zero
    LvVec3f minp{LV_FLT_MAX, LV_FLT_MAX, LV_FLT_MAX};
    //boundary box's point far from zero
    LvVec3f maxp{LV_FLT_MIN, LV_FLT_MIN, LV_FLT_MIN};
    //half size of diagonal of boundary box
    LvVec3f extent{0.f};
    //can be used as Sphere or whatever you want. This will only change by user
    float radius = 0.f;

    LvBoxBound() noexcept : LvBound(LvBoundType::BOX)
    {
    }
    LvBoxBound(const LvVec3f &min, const LvVec3f &max) noexcept
        : LvBound(LvBoundType::BOX), minp(min), maxp(max),
          extent((max - min) * 0.5f), radius(0.f)
    {
    }
    LvBoxBound(const LvVec3f &p) noexcept : LvBoxBound(p, p)
    {
    }
    LvBoxBound(const LvVec3f &center, const float _radius) noexcept
        : LvBound(LvBoundType::BOX), minp(center - LvVec3f(_radius)),
          maxp(center + LvVec3f(_radius)), extent(LvVec3f(_radius)),
          radius(_radius)
    {
    }
    LvBoxBound(const LvBoxBound &b) noexcept
        : LvBound(LvBoundType::BOX), minp(b.minp), maxp(b.maxp),
          extent(b.extent), radius(b.radius)
    {
    }

    LV_FORCEINLINE bool IsValid() noexcept override
    {
        return minp.x != LV_FLT_MAX && minp.y != LV_FLT_MAX &&
               minp.z != LV_FLT_MAX && maxp.x != LV_FLT_MIN &&
               maxp.y != LV_FLT_MIN && maxp.z != LV_FLT_MIN;
    }

    LV_FORCEINLINE void Clear() noexcept override
    {
        minp = LvVec3f{LV_FLT_MAX, LV_FLT_MAX, LV_FLT_MAX};
        maxp = LvVec3f{LV_FLT_MIN, LV_FLT_MIN, LV_FLT_MIN};
        extent = LvVec3f{0.f};
    }

    LV_FORCEINLINE void SetCenter(const LvVec3f &newCenter)
    {
        minp = newCenter - extent;
        maxp = newCenter + extent;
    }
    LV_FORCEINLINE LvVec3f GetCenter() const noexcept
    {
        return (minp + maxp) * 0.5;
    }
    LV_FORCEINLINE uint32 MaxDimension() const noexcept
    {
        return extent[0] > extent[1] ? (extent[0] > extent[2] ? 0 : 2)
                                     : (extent[1] > extent[2] ? 1 : 2);
    }

    void ExpandToInclude(const LvVec3f &p) override;
    void ExpandToInclude(const LvBound &b) override;

    template <typename... TArgs>
    void ExpandToInclude(const LvBound &b, TArgs... args)
    {
        ExpandToInclude(b);
        ExpandToInclude(args...);
    }

    bool Intersect(const LvRay &ray) const override;
    bool Intersect(const LvRay &ray, float &tnear) const override;
    bool Intersect(const LvRay &ray, float &tnear, float &tfar) const override;

    bool Intersect(const LvBoxBound &o) const;
    //line - line intersection
    bool IntersectRayAsLine(const LvRay &ray, float thresold) const;

    LV_FORCEINLINE bool Contains(const LvVec3f &v) const noexcept
    {
        bool result = true;

        result = result && minp.x <= v.x;
        result = result && minp.y <= v.y;
        result = result && minp.z <= v.z;
        result = result && v.x <= maxp.x;
        result = result && v.y <= maxp.y;
        result = result && v.z <= maxp.z;

        return result;
    }

    LV_FORCEINLINE bool Contains(const LvBoxBound &b) const noexcept
    {
        bool result = true;
        result = result && minp.x <= b.minp.x;
        result = result && minp.y <= b.minp.y;
        result = result && minp.z <= b.minp.z;
        result = result && b.maxp.x <= maxp.x;
        result = result && b.maxp.y <= maxp.y;
        result = result && b.maxp.z <= maxp.z;
        return result;
    }

    LV_FORCEINLINE float GetPerimeter() const noexcept
    {
        float wx = maxp.x - minp.x;
        float wy = maxp.y - minp.y;
        float wz = maxp.z - minp.z;

        return 4.f * (wx + wy + wz);
    }

    bool operator==(const LvBoxBound &b) const noexcept
    {
        //return (minp == b.minp) && (maxp == b.maxp);
        return minp.EpsilonEqual(b.minp, 0.f) && maxp.EpsilonEqual(b.maxp, 0.f);
    }

    bool operator!=(const LvBoxBound &rhs) const noexcept
    {
        return !operator==(rhs);
    }

    LvBoxBound &operator=(const LvBoxBound &b) noexcept
    {
        minp = b.minp;
        maxp = b.maxp;
        extent = b.extent;
        radius = b.radius;
        return *this;
    }

    void Serialize(LvArchive &serializer) override;
    void Deserialize(LvArchive &deserializer) override;
};

using BoxBound = LvBoxBound;

class LV_API LV_REFLECT LvOrientedBoxBound : public LvBound
{
public:
    //boundary box's transform (include center, axis rotation)
    LvMat4f transform{1.f};
    //boundary box's axis dir's half len
    LvVec3f extent{0.f};

    LvOrientedBoxBound() noexcept : LvBound(LvBoundType::ORIENTEDBOX)
    {
    }
    LvOrientedBoxBound(const LvMat4f &transform) noexcept
        : LvBound(LvBoundType::ORIENTEDBOX), transform(transform)
    {
    }
    LvOrientedBoxBound(const LvOrientedBoxBound &b) noexcept
        : LvBound(LvBoundType::ORIENTEDBOX), transform(b.transform),
          extent(b.extent)
    {
    }
    LvOrientedBoxBound(const LvBoxBound &b) noexcept
        : LvBound(LvBoundType::ORIENTEDBOX), extent(b.extent)
    {
        transform = LvMat4f::Translate(LvMat4f::Identity(), b.GetCenter());
    }

    LV_FORCEINLINE bool IsValid() noexcept override
    {
        return !(extent.x <= 0.f || extent.y <= 0.f || extent.z <= 0.f);
    }

    LV_FORCEINLINE void Clear() noexcept override
    {
        transform = LvMat4f{1.f};
        extent = {0.f};
    }

    LV_FORCEINLINE LvVec3f GetCenter() const noexcept
    {
        return LvVec3f{transform[3].x, transform[3].y, transform[3].z};
    }

    LV_FORCEINLINE void Transform(const LvMat4f &trans) noexcept
    {
        transform *= trans;
    }

    LV_FORCEINLINE void Translate(const LvVec3f &trans) noexcept
    {
        LvMat4f::Translate(transform, trans);
    }

    LV_FORCEINLINE void Scale(const LvVec3f &scale) noexcept
    {
        extent *= scale;
    }

    LV_FORCEINLINE uint32 MaxDimension() const noexcept
    {
        return extent[0] > extent[1] ? (extent[0] > extent[2] ? 0 : 2)
                                     : (extent[1] > extent[2] ? 1 : 2);
    }

    void ExpandToInclude(const LvVec3f &p) override;
    void ExpandToInclude(const LvBound &b) override;

    bool Intersect(const LvBound &o) const;
    bool Intersect(const LvRay &ray) const override;
    bool Intersect(const LvRay &ray, float &tNear) const override;
    bool Intersect(const LvRay &ray, float &tNear, float &tFar) const override;

    bool operator==(const LvOrientedBoxBound &b) const noexcept
    {
        return (transform == b.transform) && (extent == b.extent);
    }

    bool operator!=(const LvOrientedBoxBound &rhs) const noexcept
    {
        return !operator==(rhs);
    }

    LvOrientedBoxBound &operator=(const LvOrientedBoxBound &b) noexcept
    {
        transform = b.transform;
        extent = b.extent;
        return *this;
    }

    void Serialize(LvArchive &serializer) override;
    void Deserialize(LvArchive &serializer) override;
};

using OrientedBoxBound = LvOrientedBoxBound;

class LV_API LvMeshBound : public LvBound
{
public:
    struct HalfEdgeMesh
    {
        //TODO
    };

    LvMeshBound(LvMesh *mesh, bool isConvex = false)
        : LvBound(LvBoundType::MESH), _mesh(mesh), _isConvex(isConvex)
    {
        if (_isConvex)
            buildConvex();
    }

    LvMeshBound(const LvMeshBound &b) noexcept
        : LvBound(LvBoundType::MESH), _mesh(b._mesh), _isConvex(b._isConvex),
          _convexVertices(b._convexVertices), _convexIndices(b._convexIndices)
    {
    }

    LV_FORCEINLINE bool IsValid() noexcept override
    {
        return _mesh != nullptr;
    }

    LV_FORCEINLINE void Clear() noexcept override
    {
        _mesh = nullptr;
        _isConvex = false;
    }

    void ExpandToInclude(const LvVec3f &p) override;
    void ExpandToInclude(const LvBound &bound) override;

    bool Intersect(const LvRay &ray) const override;
    bool Intersect(const LvRay &ray, float &tNear) const override;
    bool Intersect(const LvRay &ray, float &tNear, float &tFar) const override;

    void Serialize(LvArchive &archive) override;
    void Deserialize(LvArchive &archive) override;

    void SetConvex(bool isConvex) noexcept
    {
        _isConvex = isConvex;
    }
    void SetWorldMatrix(const LvMat4f &worldMtx)
    {
        _localToWorld = worldMtx;
    }
    bool IsConvex() const noexcept
    {
        return _isConvex;
    }

private:
    void buildConvex();

    LvMesh *_mesh = nullptr;
    bool _isConvex = false;

    LvList<LvVec3f> _convexVertices;
    LvList<uint32> _convexIndices;
    LvMat4f _localToWorld;
};

using MeshBound = LvMeshBound;

LV_FORCEINLINE static bool Intersect(const LvBoxBound &box1,
                                     const LvBoxBound &box2)
{
    return !(box1.minp.x > box2.maxp.x || box1.maxp.x < box2.minp.x ||
             box1.minp.y > box2.maxp.y || box1.maxp.y < box2.minp.y ||
             box1.minp.z > box2.maxp.z || box1.maxp.z < box2.minp.z);
}

// True : sphere in Frustum or intersecting with each other
// False : sphere outside Frustum
LV_FORCEINLINE static bool IntersectSphere(const LvFrustum &frustum,
                                           const LvBoxBound &sphere)
{
    bool result = true;

    for (uint8 i = 0; i < 6; ++i)
    {
        float dist = frustum.planes[i].Distance(sphere.GetCenter());

        if (dist < -sphere.radius)
            return false; // Outside
        else if (dist < sphere.radius)
            result = true; // Intersect
    }

    return result; // Inside
}

// True : AABB in Frustum or intersecting with each other
// False : AABB outside Frustum
LV_FORCEINLINE static bool IntersectBox(const LvFrustum &frustum,
                                        const LvBoxBound &bound)
{
    bool result = true;

    LvVec3f maxV = bound.maxp;
    LvVec3f minV = bound.minp;
    LvVec3f pv, nv;

    for (uint8 i = 0; i < 6; ++i)
    {
        const LvPlane &p = frustum.planes[i];
        pv = minV;
        nv = maxV;

        if (p.x >= 0)
        {
            pv.x = maxV.x;
            nv.x = minV.x;
        }

        if (p.y >= 0)
        {
            pv.y = maxV.y;
            nv.y = minV.y;
        }

        if (p.z >= 0)
        {
            pv.z = maxV.z;
            nv.z = minV.z;
        }

        if (p.Distance(pv) < 0)
            return false; // OutSide
        else if (p.Distance(nv) < 0)
            result = true; // Intersect
    }

    return result; // Inside
}

LV_FORCEINLINE static bool IntersectSIMDAABB(const LvFrustum &frustum,
                                             const LvBoxBound &bound)
{
    const LvSimd128f origin = lv_simd_load_float3(&bound.GetCenter()[0]);
    const LvSimd128f extent = lv_simd_load_float3(&bound.extent.x);

    const LvSimd128f planeX0 = frustum.SIMDPlanes[0].m;
    const LvSimd128f planeY0 = frustum.SIMDPlanes[1].m;
    const LvSimd128f planeZ0 = frustum.SIMDPlanes[2].m;
    const LvSimd128f planeW0 = frustum.SIMDPlanes[3].m;

    const LvSimd128f planeX1 = frustum.SIMDPlanes[4].m;
    const LvSimd128f planeY1 = frustum.SIMDPlanes[5].m;
    const LvSimd128f planeZ1 = frustum.SIMDPlanes[6].m;
    const LvSimd128f planeW1 = frustum.SIMDPlanes[7].m;

    LvSimd128f originX = lv_simd_replicate(origin, 0);
    LvSimd128f originY = lv_simd_replicate(origin, 1);
    LvSimd128f originZ = lv_simd_replicate(origin, 2);

    LvSimd128f extentX = lv_simd_replicate(extent, 0);
    LvSimd128f extentY = lv_simd_replicate(extent, 1);
    LvSimd128f extentZ = lv_simd_replicate(extent, 2);

    LvSimd128f DistX_0 = lv_simd_mul(originX, planeX0);
    LvSimd128f DistY_0 = lv_simd_mul_add(originY, planeY0, DistX_0);
    LvSimd128f DistZ_0 = lv_simd_mul_add(originZ, planeZ0, DistY_0);
    LvSimd128f Distance_0 = lv_simd_sub(DistZ_0, planeW0);

    LvSimd128f MaxRadiusX_0 = lv_simd_mul(extentX, lv_simd_abs(planeX0));
    LvSimd128f MaxRadiusY_0 =
        lv_simd_mul_add(extentY, lv_simd_abs(planeY0), MaxRadiusX_0);
    LvSimd128f MaxRadius_0 =
        lv_simd_mul_add(extentZ, lv_simd_abs(planeZ0), MaxRadiusY_0);

    if (lv_simd_any_gt(Distance_0, MaxRadius_0))
        return false;

    LvSimd128f DistX_1 = lv_simd_mul(originX, planeX1);
    LvSimd128f DistY_1 = lv_simd_mul_add(originY, planeY1, DistX_1);
    LvSimd128f DistZ_1 = lv_simd_mul_add(originZ, planeZ1, DistY_1);
    LvSimd128f Distance_1 = lv_simd_sub(DistZ_1, planeW1);

    LvSimd128f MaxRadiusX_1 = lv_simd_mul(extentX, lv_simd_abs(planeX1));
    LvSimd128f MaxRadiusY_1 =
        lv_simd_mul_add(extentY, lv_simd_abs(planeY1), MaxRadiusX_1);
    LvSimd128f MaxRadius_1 =
        lv_simd_mul_add(extentZ, lv_simd_abs(planeZ1), MaxRadiusY_1);

    if (lv_simd_any_gt(Distance_1, MaxRadius_1))
        return false;

    return true;
}

LV_NS_ENGINE_END
#endif
