#pragma once

#include "Hazel/Core/DataStructure/StackLinkedList.h"
#include "Hazel/Core/Allocation/Allocator/Allocable.h"
#include "hzpch.h"

namespace Hazel
{

class PoolAllocator : public Allocable
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

    size_t m_TotalSize;
    size_t m_Used;
    size_t m_Peak;
    size_t m_Alignment;

public:
    PoolAllocator(const size_t totalSize,
                  const size_t chunkSize,
                  const size_t alignment = 4);
    virtual ~PoolAllocator();
    virtual void *Allocate(size_t size,
                           const char *flie = nullptr,
                           size_t line = 0);
    // @brief TODO 구현해야 한다.
    virtual void *Reallocate(void *ptr,
                             size_t size,
                             const char *flie = nullptr,
                             size_t line = 0)
    {
        return nullptr;
    };
    virtual void Free(void *ptr);
    virtual void Init();
    virtual void Reset();
};
} // namespace Hazel