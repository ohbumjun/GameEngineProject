#pragma once
#include "MemoryPoolInfo.h"
#include "MemoryPool.h"

class MemoryPoolManager
{
	friend class CScene;
	friend class CGameObjectFactory;
protected:
	MemoryPoolManager();
	virtual ~MemoryPoolManager();
private:
	// std::unordered_map<size_t, CMemoryPool*> m_mapPoolAllocPool;
	std::unordered_map<std::string, MemoryPool*> m_mapPoolAllocPool;
	MemoryPool* m_StackMemoryPool;
	MemoryPool* m_FreeListMemoryPool;
	class CScene* m_Scene;
private:
	// CMemoryPool* FindPoolAllocMemoryPool(const size_t ObjectTypeID);
	MemoryPool* FindPoolAllocMemoryPool(const std::string& TypeName);
	MemoryPool* FindMemoryPool(MemoryPoolType Type);

	template<typename T>
	void CreatePoolAllocMemoryPool(const char* Name, int initNum, MemoryPoolType Type)
	{
		MemoryPool* NewMemoryPool = new MemoryPool;

		NewMemoryPool->SetName(Name);

		NewMemoryPool->InitPoolAlloc<T>(initNum, Type);

		// m_mapPoolAllocPool.insert(std::make_pair(typeid(T).hash_code(), NewMemoryPool));
		m_mapPoolAllocPool.insert(std::make_pair(typeid(T).name(), NewMemoryPool));
	}
};

