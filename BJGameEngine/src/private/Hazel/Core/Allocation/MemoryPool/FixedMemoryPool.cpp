#include "Hazel/Core/Allocation/MemoryPool/FixedMemoryPool.h"
#include "Hazel/Core/Allocation/MemoryPool/MemoryPoolInfo.h"
#include "hzpch.h"

FixedMemoryPool::FixedMemoryPool()
{
}

FixedMemoryPool::~FixedMemoryPool()
{
    if (m_Allocator)
        delete m_Allocator;
}

void *FixedMemoryPool::Allocate(const size_t allocateSize)
{
    // return m_Allocator->Allocate(allocateSize, MemoryPoolInfo::ALIGNMENT);
    return m_Allocator->Allocate(m_SingleDataSize, MemoryPoolInfo::ALIGNMENT);
}

void FixedMemoryPool::Init(const uint TotalNumber, int SingleSize)
{
    if (m_Allocator)
    {
        return;
    }

    size_t AllocTotalSize = (size_t)(TotalNumber * SingleSize);

    m_Allocator = new PoolAllocator(AllocTotalSize, SingleSize);

    // m_Alignment = MemoryPoolInfo::ALIGNMENT;

    if (m_Allocator == nullptr)
        assert(false);

    m_SingleDataSize = SingleSize;

    m_Allocator->Init();
}

void FixedMemoryPool::onFree(void *ptr)
{
    m_Allocator->Free(ptr);
}
