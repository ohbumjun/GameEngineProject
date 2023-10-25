#include "hzpch.h"
#include "MemoryPool.h"

void* MemoryPool::AllocateFromPoolAlloc()
{
    return m_Allocator->Allocate(m_SingleDataSize, MemoryPoolInfo::ALIGNMENT);
}

void* MemoryPool::Allocate(const size_t allocateSize)
{
    return m_Allocator->Allocate(allocateSize, m_Alignment);
}

MemoryPool::MemoryPool()  : m_AllocatorType(MemoryPoolType::Pool)
{
}

MemoryPool::~MemoryPool()
{
    if (m_Allocator) delete m_Allocator;
}

bool MemoryPool::InitAlloc(const int TotalMemorySize, MemoryPoolType Type, const int alignment)
{
    switch (Type)
    {
    case MemoryPoolType::FreeList:
        m_Allocator = new FreeListAllocator(TotalMemorySize, FreeListAllocatorPlacementPolicy::FIND_BEST);
        break;
    case MemoryPoolType::Stack:
        m_Allocator = new StackAllocator(TotalMemorySize);
        break;
    default:
        assert(false);
        break;
    }

    m_Alignment = alignment;

    if (m_Allocator == nullptr)
        assert(false);

    m_Allocator->Init();

    return true;
}