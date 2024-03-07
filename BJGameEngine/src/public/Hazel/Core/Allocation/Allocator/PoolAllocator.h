#pragma once

#include "Hazel/Core/DataStructure/StackLinkedList.h"
#include "MemoryPoolAllocator.h"
#include "hzpch.h"

class PoolAllocator : public MemoryPoolAllocator
{
private:
    struct FreeHeader
    {
    };
    typedef StackLinkedList<FreeHeader>::Node Node;
    StackLinkedList<FreeHeader> m_FreeList;
    std::stack<size_t> m_StkAddress;
    void *m_StartPtr;
    size_t m_ChunkSize;

public:
    PoolAllocator(const size_t totalSize, const size_t chunkSize);
    virtual ~PoolAllocator();
    virtual void *Allocate(const size_t allocateSize, const size_t alignment);
    virtual void Free(void *ptr);
    virtual void Init();
    virtual void Reset();
};
