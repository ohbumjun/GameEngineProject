#include "Hazel/Core/Allocation/MemoryPool/VariantMemoryPool.h"
#include "Hazel/Core/Allocation/Allocator/FreeListAllocator.h"
#include "hzpch.h"


VariantMemoryPool::VariantMemoryPool()
{
}

VariantMemoryPool::~VariantMemoryPool()
{
}

void *VariantMemoryPool::Allocate(const size_t allocateSize)
{
    return nullptr;
}

void VariantMemoryPool::Init(uint TotalSize,
                             FreeListAllocatorPlacementPolicy policy)
{
    m_Allocator = new FreeListAllocator(TotalSize, policy);
}

void VariantMemoryPool::onFree(void *ptr)
{
    m_Allocator->Free(ptr);
}
