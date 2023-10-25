#include "hzpch.h"
#include "MemoryPoolManager.h"

MemoryPoolManager::MemoryPoolManager()
{
	// Stack Allocator 를 활용하는 Memory Pool;
	m_StackMemoryPool = new MemoryPool;
	m_StackMemoryPool->SetName("StackMemoryPool");
	m_StackMemoryPool->InitAlloc(4096, MemoryPoolType::Stack, MemoryPoolInfo::ALIGNMENT * 2);

	// FreeList
	m_FreeListMemoryPool = new MemoryPool;
	m_FreeListMemoryPool->SetName("FreeListMemoryPool");
	m_FreeListMemoryPool->InitAlloc(4096 * 2, MemoryPoolType::FreeList);
}

MemoryPoolManager::~MemoryPoolManager()
{
	auto iter = m_mapPoolAllocPool.begin();
	auto iterEnd = m_mapPoolAllocPool.end();

	for (; iter != iterEnd;)
	{
		if (iter->second) delete iter->second;
		iter = m_mapPoolAllocPool.erase(iter);
		iterEnd = m_mapPoolAllocPool.end();
	}

	if(m_FreeListMemoryPool) delete m_FreeListMemoryPool;
	if(m_StackMemoryPool) delete m_StackMemoryPool;
}

// CMemoryPool* MemoryPoolManager::FindPoolAllocMemoryPool(const size_t ObjectTypeID)
MemoryPool* MemoryPoolManager::FindPoolAllocMemoryPool(const std::string& TypeName)
{
	auto iter = m_mapPoolAllocPool.find(TypeName);

	if (iter == m_mapPoolAllocPool.end())
		return nullptr;

	return iter->second;
}

MemoryPool* MemoryPoolManager::FindMemoryPool(MemoryPoolType Type)
{
	// Pool Allocator 의 경우, m_mapPoolAllocMemoryPool 에서 찾을 것이다.
	if (Type == MemoryPoolType::Pool)
		assert(false);

	if (Type == MemoryPoolType::Stack)
		return m_StackMemoryPool;

	if (Type == MemoryPoolType::FreeList)
		return m_FreeListMemoryPool;

	assert(false);

	return nullptr;
}

