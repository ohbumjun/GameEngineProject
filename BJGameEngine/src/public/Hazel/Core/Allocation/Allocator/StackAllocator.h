#pragma once

#include "MemoryPoolAllocator.h"
#include <cstddef>

class StackAllocator : public MemoryPoolAllocator
{
    typedef unsigned char byte;

    struct AllocationHeader
    {
        // char padding;
        size_t padding;

        AllocationHeader(size_t pad) : padding(pad)
        {
        }
    };

    friend class CMemoryPool;

public:
    StackAllocator(size_t totalSize);
    ~StackAllocator();
    virtual void *Allocate(const size_t allocSize,
                           const size_t alignment) override;
    virtual void Free(void *ptr);
    virtual void Init();
    void Reset();
    void ResizeAlloc();

private:
    size_t m_Offset;
    //  void* m_StartPtr;
    byte *m_StartPtr;
    //char m_LastPadding;
    std::vector<size_t> vecAdds;
};
