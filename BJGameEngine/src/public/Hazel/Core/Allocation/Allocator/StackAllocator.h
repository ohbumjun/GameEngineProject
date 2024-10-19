#pragma once

#include "MemoryPoolAllocator.h"
#include "Hazel/Core/Allocation/Allocator/Allocable.h"
#include <cstddef>

namespace Hazel
{


class StackAllocator : public Allocable
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
    StackAllocator(size_t totalSize, size_t alignment = 4);
    ~StackAllocator();
    virtual void *Allocate(const size_t allocSize,
                           const char *flie = nullptr,
                           size_t line = 0) override;
    virtual void Free(void *ptr);
    virtual void Init();
    void Reset();
    void ResizeAlloc();

private:
    size_t m_Alignment;
    size_t m_Offset;
    //  void* m_StartPtr;
    byte *m_StartPtr;
    //char m_LastPadding;
    std::vector<size_t> vecAdds;
    size_t m_TotalSize;
    size_t m_Used;
    size_t m_Peak;
};
} // namespace Hazel
