#pragma once
#ifndef __LV_BVH_H__
#define __LV_BVH_H__

#include "system/LvTree.h"
#include "system/LvList.h"
#include "system/LvString.h"
#include "system/math/LvVec3f.h"

#include "engine/LvGeometry.h"
#include "engine/LvBound.h"

LV_NS_ENGINE_BEGIN

struct LvIntersection;
typedef LvIntersection(*DetectingIntersect)(const LvBoxBound& bound, const LvRay& ray, uint32 id);
typedef LvIntersection(*RankingIntersect)(const LvIntersection& a, const LvIntersection& b);

struct LV_API LvIntersection
{
	//t is Count of dir from origin to hit point
	float t = LV_FLT_MAX;

	LvBoxBound bound;

	uint32 id;
    
	LvIntersection() = default;
	LvIntersection(float _t, const LvBoxBound& o, uint32 id = 0)
		: t(_t)
		, bound(o)
		, id(id) 
	{ }

	LV_FORCEINLINE LvVec3f GetHitPos(const LvRay& ray) const noexcept { return ray.origin + ray.dir * t; }
	LV_FORCEINLINE operator bool() const noexcept { return t != LV_FLT_MAX; }
	LV_FORCEINLINE LvIntersection& operator=(const LvIntersection& i) noexcept { t = i.t; bound = i.bound; id = i.id; return *this; }

	LvIntersection operator()(const LvBoxBound& bound, const LvRay& ray, uint32 id, DetectingIntersect func = BasicDetectingIntersect) const noexcept;

	//Basic RankingIntersect
	LV_FORCEINLINE static LvIntersection Closest(const LvIntersection& a, const LvIntersection& b) noexcept
	{ 
		return a.t < b.t ? a : b; 
	}

	static LvIntersection BasicDetectingIntersect(const LvBoxBound& bound, const LvRay& ray, uint32 id);
};

using Intersection = LvIntersection;

class LV_API LvBVH
{
public:
	struct Pair
	{
		uint32 index = -1; //only for fast access
		LvBoxBound b;

		Pair() { }
		Pair(const LvBoxBound& _b) : b(_b) { }
		bool operator== (const Pair& pair) const { return pair.b == b; }
	};

	LvBVH();
	LvBVH(LvBoxBound* datas, const uint32 size, LvList<uint32>& outIndices);

	~LvBVH();

	/**
	* @brief Insert by data. It will potentially increase entire node of bvh tree
	* @param data data to Insert
	* @return return index of inserted data
	*/
	uint32 Insert(const LvBoxBound& data) noexcept;

	/**
	* @brief remove by data. Very slow due to find. Recommand to use remove by index
	* @param data data to remove, if doesn't contain data, return false
	* @return return success to remove or not
	*/
	///deprecated
	bool Remove(const LvBoxBound& data) noexcept;
	

	/**
	* @brief remove by indx
	* @param id index of data
	* @return return success to remove or not
	*/
	bool Remove(uint32 id) noexcept;

	/**
	* @brief datas를 BVH tree로 변환합니다.
	* @details datas의 순서는 sah에 의해 변경될 수도 있습니다. 그러나 outIndices는 처음 넣어진 datas의 순서대로 반환합니다.
	* @param datas data array
	* @param size array size
	* @param outIndices 각 data들의 id
	*/
	void Rebuild(LvBoxBound* datas, const uint32 size, LvList<uint32>& outIndices) noexcept;

	/**
	* @brief boundary box 처리가 아닌 ray와 hit하는 가장 가까운 primitive intersection을 반환합니다.
	* @param ray 검출하고자 하는 ray
	* @return 충돌 처리된 가장 가까운 primitive intersection 입니다.
	*/
	LvIntersection Intersect(const LvRay& ray) noexcept;
	
	/**
	* @brief boundary box 처리가 아닌 ray와 hit하는 func를 만족시키는 primitive intersection을 반환합니다.
	* @param ray 검출하고자 하는 ray
	* @param ranker 검출된 primtive intersection들의 rank를 결정하는 함수 포인터입니다.
	* @return 충돌 처리된 가장 rank가 높은 primitive intersection 입니다.
	*/
	LvIntersection Intersect(const LvRay& ray, RankingIntersect ranker) noexcept;

	LV_FORCEINLINE void Clear() noexcept { _leafCount = 0; _leafSize = 4; _tree.Clear(); _detector = LvIntersection::BasicDetectingIntersect; }
	LV_FORCEINLINE void SetIntersector(DetectingIntersect intersect) noexcept { _detector = intersect; }
	LV_FORCEINLINE void ClearIntersector() { _detector = LvIntersection::BasicDetectingIntersect; }
	LV_FORCEINLINE void SetLeafSize(uint32 size) noexcept{ _leafSize = size; }
	LV_FORCEINLINE constexpr uint32 Count() const noexcept{ return _leafCount; }
	LV_FORCEINLINE LvFastLinearTree<Pair>& GetTree() noexcept { return _tree; }
	
private:
	void update(const uint32 id) noexcept;

	struct BuildNode
	{
		uint32 parent, start, end;
	};

	uint32 _leafCount = 0;
	uint32 _leafSize = 4;
	LvFastLinearTree<Pair> _tree;
	DetectingIntersect _detector = LvIntersection::BasicDetectingIntersect;
};

using BVH = LvBVH;

class LV_API LvDynamicAABBTree
{
	static const float aabbExtension;
	static const int nodeNull;

	struct TreeNode
	{
		TreeNode() { }

		bool isLeaf(void) const
		{
			// The right leaf does not use the same memory as the userdata
			return left == nodeNull;
		}

		// Fat AABB for leafs, bounding AABB for branches
		LvBoxBound aabb;

		union
		{
			int parent = 0;
			int next; // free list
		};

		// Child indices
		int left = 0;
		int right = 0;

		void* userData = nullptr;

		// leaf = 0, free nodes = -1
		int height = 0;
	};

public:
	LvDynamicAABBTree();

	LvDynamicAABBTree(int nodeCapacity);

	~LvDynamicAABBTree();

	int CreateProxy(const LvBoxBound& aabb, void* userData);

	void DestroyProxy(int proxyId);

	// this method is the same as MoveProxy in the box2D.
	// I changed the name for the more intuition
	bool UpdateProxy(int proxyId, const LvBoxBound& aabb, const LvVec3f displacement);

	// Version with no displacement prediction
	bool UpdateProxy(int proxyId, const LvBoxBound& aabb);

	void* GetUserData(int proxyId) const;

	const LvBoxBound& GetFatAABB(int proxyId) const;

	template<typename T>
	void Query(T* callback, const LvBoxBound& aabb)
	{
		if (_stackCapacity < _nodeCount)
		{
			int* rp = (int*)lv_realloc(_stack, sizeof(int) * _nodeCount);
			LV_CHECK(rp != nullptr, "Fail to realloc");
			_stack = rp;
			_stackCapacity = _nodeCount;
		}

		memset(_stack, 0, sizeof(int) * _stackCapacity);

		_stack[0] = _root;

		int count = 1;
		while (count)
		{
			LV_CHECK(count < _stackCapacity, "count is more than stack capacity");
			// Pop from the stack
			int nodeId = _stack[--count];

			if (nodeId == nodeNull)
				continue;

			const TreeNode* node = _nodes + nodeId;

			if (Intersect(node->aabb, aabb) == true)
			{
				if (node->isLeaf())
				{
					bool proceed = callback->QueryCallback(nodeId);
					if (proceed == false)
						return;
				}
				else
				{
					_stack[count++] = node->left;
					_stack[count++] = node->right;
				}
			}
		}
	}

	template<typename T>
	void Raycast(T* callback, const LvRay& ray)
	{
		if (_stackCapacity < _nodeCount)
		{
			int* rp = (int*)lv_realloc(_stack, sizeof(int) * _nodeCount);
			LV_CHECK(rp != nullptr, "Fail to realloc");
			_stack = rp;
			_stackCapacity = _nodeCount;
		}

		memset(_stack, 0, sizeof(int) * _stackCapacity);

		_stack[0] = _root;

		int count = 1;
		while (count)
		{
			LV_CHECK(count < _stackCapacity, "count is more than stack capacity");
			// Pop from the stack
			int nodeId = _stack[--count];

			if (nodeId == nodeNull)
				continue;

			const TreeNode* node = _nodes + nodeId;

			if (node->aabb.Intersect(ray) == true)
			{
				if (node->isLeaf())
				{
					bool proceed = callback->RayCastCallback(ray, nodeId);
					if (proceed == false)
						return;
				}
				else
				{
					_stack[count++] = node->left;
					_stack[count++] = node->right;
				}
			}
		}
	}

	int GetHeight() const;

	// remove all data in DynamicAABBTnree
	void Clear();

private:

	int allocateNode();

	void freeNode(int nodeId);

	void insertLeaf(int leaf);

	void removeLeaf(int leaf);

	int makeBalance(int iA);

	int _root;

	TreeNode* _nodes;
	int _nodeCount;
	int _nodeCapacity;

	int _freeList;

	int _insertionCount;

	int* _stack;
	int _stackCapacity;
};


LV_NS_ENGINE_END

#endif
