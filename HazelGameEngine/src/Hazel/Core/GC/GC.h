#pragma once

#include "hzpch.h"

class TypeInfo;
class GCAllocator;
class GCObject;

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
	void reset();
	void mark();
	void sweep();
	void markRecursively(GCObject* object);
	GCObject* getFirstRootObject();
	GCObject* getNextRootObject(GCObject* curGCObject);
	GCAllocator* m_GCAllocator;
	std::list<GCObject*> m_CollectTargets;
	std::mutex m_Mutex;
};

