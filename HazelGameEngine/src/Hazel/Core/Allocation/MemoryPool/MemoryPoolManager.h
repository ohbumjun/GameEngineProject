#pragma once
#include "MemoryPoolInfo.h"
#include "FixedMemoryPool.h"
#include "VariantMemoryPool.h"

// 전역으로 사용하는 MemoryPool
class MemoryPoolManager
{
public :
	MemoryPoolManager();
	virtual ~MemoryPoolManager();
	template<typename T>
	void Allocate();
	template<typename T>
	void Free(void* dataPtr);
	void SetFixedSize(uint size)
	{
		m_FixedSize = size;
	}
private :
	FixedMemoryPool* m_FixedPool;
	VariantMemoryPool* m_VariantPool;
	uint m_FixedSize = 256;
};

template<typename T>
inline void MemoryPoolManager::Allocate()
{
	// 256 이하라면, Pool Allocator 에서 할당
	// 그 이상이라면, FreeList
	const uint dataSize = sizeof(T);

	if (dataSize <= m_FixedSize)
	{
		m_FixedPool->Allocate(dataSize);
	}
	else
	{
		m_VariantPool->Allocate(dataSize);
	}
}

template<typename T>
inline void MemoryPoolManager::Free(void* dataPtr)
{
	// 256 이하라면, Pool Allocator 에서 할당
	// 그 이상이라면, FreeList
	const uint dataSize = sizeof(T);

	if (dataSize <= m_FixedSize)
	{
		m_FixedPool->Free(dataPtr);
	}
	else
	{
		m_VariantPool->Free(dataPtr);
	}
}
