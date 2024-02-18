#pragma once

#include "hzpch.h"
#include "../Allocation/MemoryPool/MemoryPoolManager.h"
#include "GCObject.h"

class TypeInfo;
class GCAllocator;

// Single ton 객체를 활용할 것이다.
class GC
{
public :
	GC();
	~GC();
	void Run();
	void SetRoot(GCObject* gcObject, bool isRoot);
	template<typename T>
	void Allocate(bool isRoot);
	void Free(GCObject* gcObject);
	void AddCollectTarget(GCObject* object);
	GCObject* FindGCObject(void* dataPtr);
private :
	uint fixedSize = sizeof(GCObject);
	void reset();
	void mark();
	void sweep();
	void markRecursively(GCObject* object);
	GCObject* getFirstRootObject();
	GCObject* getNextRootObject(GCObject* curGCObject);
	class MemoryPoolManager* m_GCAllocator;
	std::list<GCObject*> m_CollectTargets;
	std::mutex m_Mutex;
};

