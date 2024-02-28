#include "LvPrecompiled.h"
#include "engine/LvBound.h"

#include "engine/LvMesh.h"


using namespace Lv;

LV_NS_ENGINE_BEGIN

#pragma region LvBoxBound
void LvBoxBound::ExpandToInclude(const LvVec3f& p)
{
	minp = LvVec3f::Min(minp, p);
	maxp = LvVec3f::Max(maxp, p);
	extent = LvVec3f::Abs((maxp - minp) * 0.5f);
	radius = LvVec3f::Length(extent);
}

void LvBoxBound::ExpandToInclude(const LvBound& b)
{
	switch (b.type)
	{
	case LvBoundType::BOX:
	{
		const LvBoxBound& box = *(LvBoxBound*)&b;
		minp = LvVec3f::Min(minp, box.minp);
		maxp = LvVec3f::Max(maxp, box.maxp);
		extent = LvVec3f::Abs((maxp - minp) * 0.5f);
		radius = LvVec3f::Length(extent);
		break;
	}
	default:
		LV_DEBUG_BREAK(); //NOT IMPLEMENTED YET
		break;
	}
	
}

bool LvBoxBound::Intersect(const LvRay& ray) const
{
	constexpr float rayDirEpsilon = 0.0001f;
	float tmin = LV_FLT_MIN;
	float tmax = LV_FLT_MAX;

	// For all three slabs
	for (int i = 0; i < 3; ++i)
	{
		if (LvMath::Abs(ray.dir[i]) < rayDirEpsilon)
		{
			// Ray is parallel to slab. No hit if origin not within slab
			if (ray.origin[i] < minp[i] || ray.origin[i] > maxp[i]) return false;
		}
		else
		{
			// Compute intersection t value of ray with near and far plane of slab
			float t1 = (minp[i] - ray.origin[i]) * ray.invDir[i];
			float t2 = (maxp[i] - ray.origin[i]) * ray.invDir[i];

			// Make t1 be intersection with near plane, t2 with far plane
			if (t1 > t2) LV_SWAP(t1, t2, float);

			// Compute the intersection of slab intersection intervals
			tmin = LV_MAX(tmin, t1);
			tmax = LV_MIN(tmax, t2);

			// Exit with no collision as soon as slab intersection becomes empty
			if (tmin > tmax) return false;
		}
	}

	// Ray intersects all 3 slabs.
	return true;
}

bool LvBoxBound::Intersect(const LvRay& ray, float& tnear) const
{
	float txminp = (minp.x - ray.origin.x) * ray.invDir.x;
	float txmaxp = (maxp.x - ray.origin.x) * ray.invDir.x;
	if (txminp > txmaxp) LV_SWAP(txminp, txmaxp, float);

	float tyminp = (minp.y - ray.origin.y) * ray.invDir.y;
	float tymaxp = (maxp.y - ray.origin.y) * ray.invDir.y;
	if (tyminp > tymaxp) LV_SWAP(tyminp, tymaxp, float);

	if (txminp > tymaxp || tyminp > txmaxp) return false;

	float tminp = LV_MAX(txminp, tyminp);
	float tmaxp = LV_MIN(txmaxp, tymaxp);
	float tzminp = (minp.z - ray.origin.z) * ray.invDir.z;
	float tzmaxp = (maxp.z - ray.origin.z) * ray.invDir.z;
	if (tzminp > tzmaxp) LV_SWAP(tzminp, tzmaxp, float);

	if (tzminp > tmaxp || tminp > tzmaxp) return false;
	if (isinf(tminp) || isinf(tmaxp) || isinf(tzminp) || isinf(tzmaxp)) return false;

	tnear = LV_MAX(tminp, tzminp);
	return true;
}

bool LvBoxBound::Intersect(const LvRay& ray, float& tnear, float& tfar) const
{
	float txminp = (minp.x - ray.origin.x) * ray.invDir.x;
	float txmaxp = (maxp.x - ray.origin.x) * ray.invDir.x;
	if (txminp > txmaxp) LV_SWAP(txminp, txmaxp, float);

	float tyminp = (minp.y - ray.origin.y) * ray.invDir.y;
	float tymaxp = (maxp.y - ray.origin.y) * ray.invDir.y;
	if (tyminp > tymaxp) LV_SWAP(tyminp, tymaxp, float);

	if (txminp > tymaxp || tyminp > txmaxp) return false;

	float tminp = LV_MAX(txminp, tyminp);
	float tmaxp = LV_MIN(txmaxp, tymaxp);
	float tzminp = (minp.z - ray.origin.z) * ray.invDir.z;
	float tzmaxp = (maxp.z - ray.origin.z) * ray.invDir.z;
	if (tzminp > tzmaxp) LV_SWAP(tzminp, tzmaxp, float);

	if (tzminp > tmaxp || tminp > tzmaxp) return false;
	if (isinf(tminp) || isinf(tmaxp) || isinf(tzminp) || isinf(tzmaxp)) return false;

	tnear = LV_MAX(tminp, tzminp);
	tfar = LV_MIN(tmaxp, tzmaxp);
	return true;
}

bool LvBoxBound::Intersect(const LvBoxBound& o) const
{
	return (
		(this->minp.x < o.maxp.x || LvMath::EpsilonEqual(this->minp.x, o.maxp.x, 1.732e-3f)) &&
		(this->maxp.x > o.minp.x || LvMath::EpsilonEqual(this->maxp.x, o.minp.x, 1.732e-3f)) &&
		(this->minp.y < o.maxp.y || LvMath::EpsilonEqual(this->minp.y, o.maxp.y, 1.732e-3f)) &&
		(this->maxp.y > o.minp.y || LvMath::EpsilonEqual(this->maxp.y, o.minp.y, 1.732e-3f)) &&
		(this->minp.z < o.maxp.z || LvMath::EpsilonEqual(this->minp.z, o.maxp.z, 1.732e-3f)) &&
		(this->maxp.z > o.minp.z || LvMath::EpsilonEqual(this->maxp.z, o.minp.z, 1.732e-3f))
		);
}

bool LvBoxBound::IntersectRayAsLine(const LvRay& ray, float thresold) const
{
	LvVec3f p13, p43, p21;
	p13 = minp - ray.origin;
	p43 = ray.dir * 1000.;
	p21 = maxp - minp;

	float d1343 = LvVec3f::Dot(p13, p43);
	float d4321 = LvVec3f::Dot(p43, p21);
	float d1321 = LvVec3f::Dot(p13, p21);
	float d4343 = LvVec3f::Dot(p43, p43);
	float d2121 = LvVec3f::Dot(p21, p21);
	float denom = d2121 * d4343 - d4321 * d4321;
	if (LvMath::Abs(denom) < LV_EPS) return false;

	float numer = d1343 * d4321 - d1321 * d4343;
	float ua = numer / denom;
	float ub = (d1343 + d4321 * ua) / d4343;
	if (!(ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1)) return false;
	//thresold는 center와 line사이의 최대 간격을 의미 (이것보다 작으면 intersection 한것으로 판단)
	if (LvVec3f::Length(minp + p21 * ua - (ray.origin + p43 * ub)) > thresold) return false;

	return true;
}

void LvBoxBound::Serialize(LvArchive& archive)
{
	archive.WriteStartObject(LvReflection::GetTypeId<LvBoxBound>(), this);
	LV_WRITE(archive, minp);
	LV_WRITE(archive, maxp);
	LV_WRITE(archive, extent);
	LV_WRITE(archive, radius);
	archive.WriteEndObject();
}

void LvBoxBound::Deserialize(LvArchive& dearchive)
{
	dearchive.ReadStartObject(LvReflection::GetTypeId<LvBoxBound>(), this);
	LV_READ(dearchive, minp);
	LV_READ(dearchive, maxp);
	LV_READ(dearchive, extent);
	LV_READ(dearchive, radius);
	dearchive.ReadEndObject();
}

#pragma endregion



#pragma region LvOrientedBoxBound
void LvOrientedBoxBound::ExpandToInclude(const LvVec3f& p) 
{
	LvVec3f v = p - transform.cols[3];
	extent.x = LV_MAX(v.x, extent.x);
	extent.y = LV_MAX(v.y, extent.y);
	extent.z = LV_MAX(v.z, extent.z);
}

void LvOrientedBoxBound::ExpandToInclude(const LvBound& b)
{
	switch (b.type)
	{
	case LvBoundType::ORIENTEDBOX:
	{
		auto bb = static_cast<const LvOrientedBoxBound&>(b);

		LvVec3f v0 = LvVec3f(bb.extent.x, bb.extent.y, bb.extent.z);
		LvVec3f v1 = LvVec3f(bb.extent.x, -bb.extent.y, bb.extent.z);
		LvVec3f v2 = LvVec3f(bb.extent.x, bb.extent.y, -bb.extent.z);
		LvVec3f v3 = LvVec3f(bb.extent.x, -bb.extent.y, -bb.extent.z);

		LvVec3f v4 = LvVec3f(-bb.extent.x, bb.extent.y, bb.extent.z);
		LvVec3f v5 = LvVec3f(-bb.extent.x, -bb.extent.y, bb.extent.z);
		LvVec3f v6 = LvVec3f(-bb.extent.x, bb.extent.y, -bb.extent.z);
		LvVec3f v7 = LvVec3f(-bb.extent.x, -bb.extent.y, -bb.extent.z);

		ExpandToInclude(bb.transform.MultiplyPoint(v0) - transform.cols[3]);
		ExpandToInclude(bb.transform.MultiplyPoint(v1) - transform.cols[3]);
		ExpandToInclude(bb.transform.MultiplyPoint(v2) - transform.cols[3]);
		ExpandToInclude(bb.transform.MultiplyPoint(v3) - transform.cols[3]);

		ExpandToInclude(bb.transform.MultiplyPoint(v4) - transform.cols[3]);
		ExpandToInclude(bb.transform.MultiplyPoint(v5) - transform.cols[3]);
		ExpandToInclude(bb.transform.MultiplyPoint(v6) - transform.cols[3]);
		ExpandToInclude(bb.transform.MultiplyPoint(v7) - transform.cols[3]);
	}
	case LvBoundType::BOX:
	{
		auto bb = static_cast<const LvBoxBound&>(b);

		LvVec3f v0 = LvVec3f(bb.extent.x, bb.extent.y, bb.extent.z) - bb.GetCenter();
		LvVec3f v1 = LvVec3f(bb.extent.x, -bb.extent.y, bb.extent.z) - bb.GetCenter();
		LvVec3f v2 = LvVec3f(bb.extent.x, bb.extent.y, -bb.extent.z) - bb.GetCenter();
		LvVec3f v3 = LvVec3f(bb.extent.x, -bb.extent.y, -bb.extent.z) - bb.GetCenter();

		LvVec3f v4 = LvVec3f(-bb.extent.x, bb.extent.y, bb.extent.z) - bb.GetCenter();
		LvVec3f v5 = LvVec3f(-bb.extent.x, -bb.extent.y, bb.extent.z) - bb.GetCenter();
		LvVec3f v6 = LvVec3f(-bb.extent.x, bb.extent.y, -bb.extent.z) - bb.GetCenter();
		LvVec3f v7 = LvVec3f(-bb.extent.x, -bb.extent.y, -bb.extent.z) - bb.GetCenter();

		ExpandToInclude(v0);
		ExpandToInclude(v1);
		ExpandToInclude(v2);
		ExpandToInclude(v3);

		ExpandToInclude(v4);
		ExpandToInclude(v5);
		ExpandToInclude(v6);
		ExpandToInclude(v7);
	}
	case LvBoundType::MESH:
	{
		// should impl k-DOP Overlap Test
		break;
	}
	}
}

// Ref by Real-time collision detiection
bool TestOBBOBB(const LvOrientedBoxBound& a, const LvOrientedBoxBound& b)
{
	float ra, rb;
	LvMat3f R, AbsR;
	// Compute rotation matrix expressing b in a’s coordinate frame
	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			R[(int)i][(int)j] = LvVec3f::Dot(a.transform.cols[i], b.transform.cols[j]);
		}
	}

	// Compute translation vector t
	LvVec3f t = b.transform.cols[3] - a.transform.cols[3];
	// Bring translation into a’s coordinate frame
	t = LvVec3f(LvVec3f::Dot(t, a.transform.cols[0]), LvVec3f::Dot(t, a.transform.cols[2]), LvVec3f::Dot(t, a.transform.cols[2]));

	// Compute common subexpressions. Add in an epsilon term to
	// counteract arithmetic errors when two edges are parallel and
	// their cross product is (near) null (see text for details)
	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			AbsR[i][j] = LvMath::Abs(R[i][j]) + LV_EPS;
		}
	}

	// Test axes L = A0, L = A1, L = A2
	for (size_t i = 0; i < 3; i++) 
	{
		ra = a.extent[i];
		rb = b.extent[0] * AbsR[(int)i][0] + b.extent[1] * AbsR[(int)i][1] + b.extent[2] * AbsR[(int)i][2];
		if (LvMath::Abs(t[i]) > ra + rb) return false;
	}

	// Test axes L = B0, L = B1, L = B2
	for (size_t i = 0; i < 3; i++)
	{
		ra = a.extent[0] * AbsR[0][i] + a.extent[1] * AbsR[1][i] + a.extent[2] * AbsR[2][i];
		rb = b.extent[i];
		if (LvMath::Abs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb) return false;
	}

	// Test axis L = A0 x B0
	ra = a.extent[1] * AbsR[2][0] + a.extent[2] * AbsR[1][0];
	rb = b.extent[1] * AbsR[0][2] + b.extent[2] * AbsR[0][1];
	if (LvMath::Abs(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb) return false;
	// Test axis L = A0 x B1
	ra = a.extent[1] * AbsR[2][1] + a.extent[2] * AbsR[1][1];
	rb = b.extent[0] * AbsR[0][2] + b.extent[2] * AbsR[0][0];
	if (LvMath::Abs(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb) return false;
	// Test axis L = A0 x B2
	ra = a.extent[1] * AbsR[2][2] + a.extent[2] * AbsR[1][2];
	rb = b.extent[0] * AbsR[0][1] + b.extent[1] * AbsR[0][0];
	if (LvMath::Abs(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb) return false;
	// Test axis L = A1 x B0
	ra = a.extent[0] * AbsR[2][0] + a.extent[2] * AbsR[0][0];
	rb = b.extent[1] * AbsR[1][2] + b.extent[2] * AbsR[1][1];
	if (LvMath::Abs(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb) return false;
	// Test axis L = A1 x B1
	ra = a.extent[0] * AbsR[2][1] + a.extent[2] * AbsR[0][1];
	rb = b.extent[0] * AbsR[1][2] + b.extent[2] * AbsR[1][0];
	if (LvMath::Abs(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb) return false;
	// Test axis L = A1 x B2
	ra = a.extent[0] * AbsR[2][2] + a.extent[2] * AbsR[0][2];
	rb = b.extent[0] * AbsR[1][1] + b.extent[1] * AbsR[1][0];
	if (LvMath::Abs(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb) return false;
	// Test axis L = A2 x B0
	ra = a.extent[0] * AbsR[1][0] + a.extent[1] * AbsR[0][0];
	rb = b.extent[1] * AbsR[2][2] + b.extent[2] * AbsR[2][1];
	if (LvMath::Abs(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb) return false;
	// Test axis L = A2 x B1
	ra = a.extent[0] * AbsR[1][1] + a.extent[1] * AbsR[0][1];
	rb = b.extent[0] * AbsR[2][2] + b.extent[2] * AbsR[2][0];
	if (LvMath::Abs(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb) return false;
	// Test axis L = A2 x B2
	ra = a.extent[0] * AbsR[1][2] + a.extent[1] * AbsR[0][2];
	rb = b.extent[0] * AbsR[2][1] + b.extent[1] * AbsR[2][0];
	if (LvMath::Abs(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb) return false;
	// Since no separating axis is found, the OBBs must be intersecting
	return true;
}

bool LvOrientedBoxBound::Intersect(const LvBound& b) const
{
	switch (b.type)
	{
	case LvBoundType::ORIENTEDBOX:
	{
		return TestOBBOBB(*this, static_cast<const LvOrientedBoxBound&>(b));
	}
	case LvBoundType::BOX:
	{
		const LvBoxBound& bb = static_cast<const LvBoxBound&>(b);
		LvOrientedBoxBound ob(bb);
		return TestOBBOBB(*this, bb);
	}
	case LvBoundType::MESH:
	{
		// should impl k-DOP Overlap Test
		break;
	}
	}
	return false;
}

bool LvOrientedBoxBound::Intersect(const LvRay& ray) const
{
	float tMin = LV_FLT_MIN;
	float tMax = LV_FLT_MAX;

	LvVec3f position = transform[3];
	LvVec3f delta = position - ray.origin;

	LvVec3f minAABB = extent * -1.f;
	LvVec3f maxAABB = extent;

	for (int i = 0; i < 3; ++i)
	{
		LvVec3f axis = transform[i];
		float e = LvVec3f::Dot(axis, delta);
		float f = 1.f / LvVec3f::Dot(ray.dir, axis);

		if (LvMath::Abs(f) < LV_EPS)
		{
			if (e < minAABB[i] || e > maxAABB[i])
				return false;
		}
		else
		{
			float t1 = (minAABB[i] + e) * f;
			float t2 = (maxAABB[i] + e) * f;

			if (t1 > t2) LV_SWAP(t1, t2, float);
			tMin = LV_MAX(tMin, t1);
			tMax = LV_MIN(tMax, t2);
			if (tMin > tMax)
				return false;
		}
	}

	return true;
}

bool LvOrientedBoxBound::Intersect(const LvRay& ray, float& tNear) const
{
	float tMin = LV_FLT_MIN;
	float tMax = LV_FLT_MAX;

	LvVec3f position = transform[3];
	LvVec3f delta = position - ray.origin;

	LvVec3f minAABB = extent * -1.f;
	LvVec3f maxAABB = extent;

	for (int i = 0; i < 3; ++i)
	{
		LvVec3f axis = transform[i];
		float e = LvVec3f::Dot(axis, delta);
		float f = 1.f / LvVec3f::Dot(ray.dir, axis);

		if (LvMath::Abs(f) < LV_EPS)
		{
			if (e < minAABB[i] || e > maxAABB[i])
				return false;
		}
		else
		{
			float t1 = (minAABB[i] + e) * f;
			float t2 = (maxAABB[i] + e) * f;

			if (t1 > t2) LV_SWAP(t1, t2, float);
			tMin = LV_MAX(tMin, t1);
			tMax = LV_MIN(tMax, t2);
			if (tMin > tMax)
				return false;
		}
	}

	tNear = tMin;

	return true;
}


bool LvOrientedBoxBound::Intersect(const LvRay& ray, float& tNear, float& tFar) const
{
	float tMin = LV_FLT_MIN;
	float tMax = LV_FLT_MAX;

	LvVec3f position = transform[3];
	LvVec3f delta = position - ray.origin;

	LvVec3f minAABB = extent * -1.f;
	LvVec3f maxAABB = extent;

	for (int i = 0; i < 3; ++i)
	{
		LvVec3f axis = transform[i];
		float e = LvVec3f::Dot(axis, delta);
		float f = 1.f / LvVec3f::Dot(ray.dir, axis);

		if (LvMath::Abs(f) < LV_EPS)
		{
			if (e < minAABB[i] || e > maxAABB[i])
				return false;
		}
		else
		{
			float t1 = (minAABB[i] + e) * f;
			float t2 = (maxAABB[i] + e) * f;

			if (t1 > t2) LV_SWAP(t1, t2, float);
			tMin = LV_MAX(tMin, t1);
			tMax = LV_MIN(tMax, t2);
			if (tMin > tMax)
				return false;
		}
	}

	tNear = tMin;
	tFar = tMax;

	return true;
}

void LvOrientedBoxBound::Serialize(LvArchive& archive)
{
	archive.WriteStartObject(LvReflection::GetTypeId<LvOrientedBoxBound>(), this);
	LV_WRITE(archive, transform);
	LV_WRITE(archive, extent);
	archive.WriteEndObject();
}

void LvOrientedBoxBound::Deserialize(LvArchive& archive)
{
	archive.ReadStartObject(LvReflection::GetTypeId<LvOrientedBoxBound>(), this);
	LV_READ(archive, transform);
	LV_READ(archive, extent);
	archive.ReadEndObject();
}
#pragma endregion


#pragma region LvMeshBound
void LvMeshBound::ExpandToInclude(const LvVec3f& p)
{
	if (!_isConvex)
		return; //mesh bound just include mesh not other point
	
	//TODO
}

void LvMeshBound::ExpandToInclude(const LvBound& b)
{
	if (!_isConvex)
		return; //mesh bound just include mesh not other point

	//TODO
}

bool LvMeshBound::Intersect(const LvRay& ray) const
{
	if (_isConvex)
	{
		return false;
		//TODO
	}
	else
	{
		switch (_mesh->GetLayoutType())
		{
		case LvMesh::MemoryLayout::AOS:
		{
			const LvList<float>& vertices = _mesh->GetVertices();
			const LvList<uint32>& indices = _mesh->GetIndices();
			size_t vertexSize = vertices.Count() / _mesh->GetPolygonVertexCount();
			for (size_t index = 0; index < indices.Count(); index += 3)
			{
				size_t vertexIndex0 = indices[index] * vertexSize;
				size_t vertexIndex1 = indices[index + 1] * vertexSize;
				size_t vertexIndex2 = indices[index + 2] * vertexSize;

				LvVec3f t0 = _localToWorld * LvVec4f(vertices[vertexIndex0], vertices[vertexIndex0 + 1], vertices[vertexIndex0 + 2], 1.f);
				LvVec3f t1 = _localToWorld * LvVec4f(vertices[vertexIndex1], vertices[vertexIndex1 + 1], vertices[vertexIndex1 + 2], 1.f);
				LvVec3f t2 = _localToWorld * LvVec4f(vertices[vertexIndex2], vertices[vertexIndex2 + 1], vertices[vertexIndex2 + 2], 1.f);

				float t = LV_FLT_MAX;
				bool intersected = ray.IntersectTriangle(t0, t1, t2, t);

				if (intersected)
					return true;
			}
			break;
		}

		case LvMesh::MemoryLayout::SOA:
		{
			const LvList<float>& positions = _mesh->GetPositions();
			const LvList<uint32>& indices = _mesh->GetIndices();
			for (size_t index = 0; index < indices.Count(); index += 3)
			{
				size_t positionIndex0 = indices[index] * 4;
				size_t positionIndex1 = indices[index + 1] * 4;
				size_t positionIndex2 = indices[index + 2] * 4;

				LvVec3f t0 = _localToWorld * LvVec4f(positions[positionIndex0], positions[positionIndex0 + 1], positions[positionIndex0 + 2], positions[positionIndex0 + 3]);
				LvVec3f t1 = _localToWorld * LvVec4f(positions[positionIndex1], positions[positionIndex1 + 1], positions[positionIndex1 + 2], positions[positionIndex1 + 3]);
				LvVec3f t2 = _localToWorld * LvVec4f(positions[positionIndex2], positions[positionIndex2 + 1], positions[positionIndex2 + 2], positions[positionIndex2 + 3]);

				float t = LV_FLT_MAX;
				bool intersected = ray.IntersectTriangle(t0, t1, t2, t);

				if (intersected)
					return true;
			}
			break;
		}
		}
	}
	return false;
}

bool LvMeshBound::Intersect(const LvRay& ray, float& tNear) const
{
	bool hasIntersect = false;
	tNear = LV_FLT_MAX;

	if (_isConvex)
	{
		return false;
		//TODO
	}
	else
	{
		switch (_mesh->GetLayoutType())
		{
		case LvMesh::MemoryLayout::AOS:
		{
			const LvList<float>& vertices = _mesh->GetVertices();
			const LvList<uint32>& indices = _mesh->GetIndices();
			size_t vertexSize = vertices.Count() / _mesh->GetPolygonVertexCount();
			for (size_t index = 0; index < indices.Count(); index += 3)
			{
				size_t vertexIndex0 = indices[index] * vertexSize;
				size_t vertexIndex1 = indices[index + 1] * vertexSize;
				size_t vertexIndex2 = indices[index + 2] * vertexSize;

				LvVec3f t0 = _localToWorld * LvVec4f(vertices[vertexIndex0], vertices[vertexIndex0 + 1], vertices[vertexIndex0 + 2], 1.f);
				LvVec3f t1 = _localToWorld * LvVec4f(vertices[vertexIndex1], vertices[vertexIndex1 + 1], vertices[vertexIndex1 + 2], 1.f);
				LvVec3f t2 = _localToWorld * LvVec4f(vertices[vertexIndex2], vertices[vertexIndex2 + 1], vertices[vertexIndex2 + 2], 1.f);

				float t = LV_FLT_MAX;
				bool intersected = ray.IntersectTriangle(t0, t1, t2, t);

				hasIntersect |= intersected;
				if (intersected)
					tNear = LV_MIN(tNear, t);
			}
			break;
		}

		case LvMesh::MemoryLayout::SOA:
		{
			const LvList<float>& positions = _mesh->GetPositions();
			const LvList<uint32>& indices = _mesh->GetIndices();
			for (size_t index = 0; index < indices.Count(); index += 3)
			{
				size_t positionIndex0 = indices[index] * 4;
				size_t positionIndex1 = indices[index + 1] * 4;
				size_t positionIndex2 = indices[index + 2] * 4;

				LvVec3f t0 = _localToWorld * LvVec4f(positions[positionIndex0], positions[positionIndex0 + 1], positions[positionIndex0 + 2], positions[positionIndex0 + 3]);
				LvVec3f t1 = _localToWorld * LvVec4f(positions[positionIndex1], positions[positionIndex1 + 1], positions[positionIndex1 + 2], positions[positionIndex1 + 3]);
				LvVec3f t2 = _localToWorld * LvVec4f(positions[positionIndex2], positions[positionIndex2 + 1], positions[positionIndex2 + 2], positions[positionIndex2 + 3]);

				float t = LV_FLT_MAX;
				bool intersected = ray.IntersectTriangle(t0, t1, t2, t);

				hasIntersect |= intersected;
				if (intersected)
					tNear = LV_MIN(tNear, t);
			}
			break;
		}
		}
	}
	return hasIntersect;
}


bool LvMeshBound::Intersect(const LvRay& ray, float& tNear, float& tFar) const
{
	bool hasIntersect = false;
	tNear = LV_FLT_MAX;
	tFar = -1.f;

	if (_isConvex)
	{
		return false;
		//TODO
	}
	else
	{
		switch (_mesh->GetLayoutType())
		{
		case LvMesh::MemoryLayout::AOS:
		{
			const LvList<float>& vertices = _mesh->GetVertices();
			const LvList<uint32>& indices = _mesh->GetIndices();
			size_t vertexSize = vertices.Count() / _mesh->GetPolygonVertexCount();
			for (size_t index = 0; index < indices.Count(); index += 3)
			{
				size_t vertexIndex0 = indices[index] * vertexSize;
				size_t vertexIndex1 = indices[index + 1] * vertexSize;
				size_t vertexIndex2 = indices[index + 2] * vertexSize;

				LvVec3f t0 = _localToWorld * LvVec4f(vertices[vertexIndex0], vertices[vertexIndex0 + 1], vertices[vertexIndex0 + 2], 1.f);
				LvVec3f t1 = _localToWorld * LvVec4f(vertices[vertexIndex1], vertices[vertexIndex1 + 1], vertices[vertexIndex1 + 2], 1.f);
				LvVec3f t2 = _localToWorld * LvVec4f(vertices[vertexIndex2], vertices[vertexIndex2 + 1], vertices[vertexIndex2 + 2], 1.f);

				float t = LV_FLT_MAX;
				bool intersected = ray.IntersectTriangle(t0, t1, t2, t);

				hasIntersect |= intersected;
				if (intersected)
				{
					tNear = LV_MIN(tNear, t);
					tFar = LV_MAX(tFar, t);
				}
			}
			break;
		}

		case LvMesh::MemoryLayout::SOA:
		{
			const LvList<float>& positions = _mesh->GetPositions();
			const LvList<uint32>& indices = _mesh->GetIndices();
			for (size_t index = 0; index < indices.Count(); index += 3)
			{
				size_t positionIndex0 = indices[index] * 4;
				size_t positionIndex1 = indices[index + 1] * 4;
				size_t positionIndex2 = indices[index + 2] * 4;

				LvVec3f t0 = _localToWorld * LvVec4f(positions[positionIndex0], positions[positionIndex0 + 1], positions[positionIndex0 + 2], positions[positionIndex0 + 3]);
				LvVec3f t1 = _localToWorld * LvVec4f(positions[positionIndex1], positions[positionIndex1 + 1], positions[positionIndex1 + 2], positions[positionIndex1 + 3]);
				LvVec3f t2 = _localToWorld * LvVec4f(positions[positionIndex2], positions[positionIndex2 + 1], positions[positionIndex2 + 2], positions[positionIndex2 + 3]);

				float t = LV_FLT_MAX;
				bool intersected = ray.IntersectTriangle(t0, t1, t2, t);

				hasIntersect |= intersected;
				if (intersected)
				{
					tNear = LV_MIN(tNear, t);
					tFar = LV_MAX(tFar, t);
				}
			}
			break;
		}
		}
	}
	return hasIntersect;
}

void LvMeshBound::Serialize(LvArchive& archive)
{

}

void LvMeshBound::Deserialize(LvArchive& archive)
{

}

void LvMeshBound::buildConvex()
{
	return;  //????

	if (_mesh == nullptr)
		return;

	size_t vertexCount = _mesh->GetPolygonVertexCount();
	if (vertexCount <= 3) //doesn't need convex
	{
		_isConvex = false;
		return;
	}

	//find first initializing tetrahedron
	LvList<size_t> initIndices(6,6);
	LvList<LvVec3f> vec3Positions(vertexCount, vertexCount);
	{
		switch (_mesh->GetLayoutType())
		{
		case LvMesh::MemoryLayout::AOS:
		{
			const LvList<float>& vertices = _mesh->GetVertices();
			size_t vertexSize = vertices.Count() / vertexCount;
			LvList<float> extremeValues = { vertices[0], vertices[0], vertices[1], vertices[1], vertices[2], vertices[2] };
			for (size_t index = 0; index < vertices.Count(); index += vertexSize)
			{
				const float* vec3 = &vertices[index];
				size_t realIndex = index / vertexSize;
				vec3Positions[realIndex] = LvVec3f(vec3[0], vec3[1], vec3[2]);

				if (vec3[0] > extremeValues[0])
				{
					extremeValues[0] = vec3[0];
					initIndices[0] = realIndex;
				}
				else if (vec3[0] < extremeValues[1])
				{
					extremeValues[1] = vec3[0];
					initIndices[1] = realIndex;
				}
				else if (vec3[1] > extremeValues[2])
				{
					extremeValues[2] = vec3[1];
					initIndices[2] = realIndex;
				}
				else if (vec3[1] < extremeValues[3])
				{
					extremeValues[3] = vec3[1];
					initIndices[3] = realIndex;
				}
				else if (vec3[2] > extremeValues[4])
				{
					extremeValues[4] = vec3[2];
					initIndices[4] = realIndex;
				}
				else if (vec3[2] < extremeValues[5])
				{
					extremeValues[5] = vec3[2];
					initIndices[5] = realIndex;
				}
			}
			break;
		}

		case LvMesh::MemoryLayout::SOA:
		{
			const LvList<float>& positions = _mesh->GetPositions();
			LvList<float> extremeValues = { positions[0], positions[0], positions[1], positions[1], positions[2], positions[2] };
			for (size_t index = 0; index < positions.Count(); index += 4)
			{
				size_t realIndex = index / 4;
				const float* vec3 = &positions[index];
				vec3Positions[realIndex] = LvVec3f(vec3[0], vec3[1], vec3[2]);

				if (vec3[0] > extremeValues[0])
				{
					extremeValues[0] = vec3[0];
					initIndices[0] = realIndex;
				}
				else if (vec3[0] < extremeValues[1])
				{
					extremeValues[1] = vec3[0];
					initIndices[1] = realIndex;
				}
				else if (vec3[1] > extremeValues[2])
				{
					extremeValues[2] = vec3[1];
					initIndices[2] = realIndex;
				}
				else if (vec3[1] < extremeValues[3])
				{
					extremeValues[3] = vec3[1];
					initIndices[3] = realIndex;
				}
				else if (vec3[2] > extremeValues[4])
				{
					extremeValues[4] = vec3[2];
					initIndices[4] = realIndex;
				}
				else if (vec3[2] < extremeValues[5])
				{
					extremeValues[5] = vec3[2];
					initIndices[5] = realIndex;
				}
			}
			break;
		}
		}
	}

	float scale = 0.f;
	{
		for (size_t i = 0; i < 6; ++i)
		{
			const float* v = &vec3Positions[initIndices[i]].x;
			v += i / 2; //sync x, y, z
			float a = LvMath::Abs(*v);
			if (a > scale)
				scale = a;
		}
	}

	float epsilon = scale * LV_EPS;

	//remove points in tetrahedron
	{	
		//find two most distant extreme point
		float maxDistance = LV_EPS;
		size_t longestSelected[2];
		for (int i = 0; i < 6; ++i)
		{
			for (int j = i + 1; j < 6; ++j)
			{
				float distance = LvVec3f::LengthSquared(vec3Positions[initIndices[i]] - vec3Positions[initIndices[j]]);
				if (distance > maxDistance)
				{
					maxDistance = distance;
					longestSelected[0] = i;
					longestSelected[1] = j;
				}
			}
		}

		LV_CHECK(maxDistance != LV_EPS, "Error"); //every points are same or too small can't calculate convex

		//find basic plane of tetrahedron
		LvLine3D longestLine(vec3Positions[longestSelected[0]], vec3Positions[longestSelected[1]]);
		maxDistance = LV_EPS;
		size_t farthestSelected;
		for (size_t i = 0; i < vertexCount; ++i)
		{
			if (i == longestSelected[0] || i == longestSelected[1])
				continue;

			if (longestLine.Distance(vec3Positions[i]) > maxDistance)
				farthestSelected = i;
		}

		//find last point of tetrahedron
		LvVec3f center = (vec3Positions[longestSelected[0]] + vec3Positions[longestSelected[1]] + vec3Positions[farthestSelected]) / 3.f;
		maxDistance = LV_EPS;
		size_t pointSelected;
		for (size_t i = 0; i < vertexCount; ++i)
		{
			if (i == longestSelected[0] || i == longestSelected[1] || i == farthestSelected)
				continue;

			if (LvVec3f::LengthSquared(vec3Positions[i] - center) > maxDistance)
				pointSelected = i;
		}

		//enforce CCW
		//basic plane normal 계산 후 마지막 point 확인해서 ccw로 고정
		LvVec3f triangleNormal = LvVec3f::Normalize(LvVec3f::Cross(vec3Positions[longestSelected[0]] - vec3Positions[farthestSelected], vec3Positions[longestSelected[1]] - vec3Positions[farthestSelected]));
		if (LvVec3f::Dot(triangleNormal, vec3Positions[pointSelected]) + LvVec3f::Dot(-triangleNormal, vec3Positions[longestSelected[0]]) >= 0)
			LV_SWAP(longestSelected[0], longestSelected[1], size_t);

	}


}
#pragma endregion

LV_NS_ENGINE_END
