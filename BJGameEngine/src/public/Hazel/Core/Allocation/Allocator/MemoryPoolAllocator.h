#pragma once

namespace Hazel
{

class MemoryPoolAllocator
{
    friend class MemoryPool;

protected:
    size_t m_TotalSize;
    size_t m_Used;
    size_t m_Peak;

protected:
    MemoryPoolAllocator(size_t TotalSize);
    virtual ~MemoryPoolAllocator();
    virtual void *Allocate(const size_t allocationSize,
                           const size_t alignment = 0);
};
} // namespace Hazel