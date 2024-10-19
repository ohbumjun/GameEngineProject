#pragma once

namespace Hazel
{

class MemoryPoolAllocator
{
    friend class MemoryPool;

protected:

protected:
    MemoryPoolAllocator(size_t TotalSize);
    virtual ~MemoryPoolAllocator();
    virtual void *Allocate(const size_t allocationSize,
                           const size_t alignment = 0);
};
} // namespace Hazel