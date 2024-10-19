#pragma once

#include "BaseMemoryPool.h"
#include "Hazel/Core/Allocation/Allocator/PoolAllocator.h"
#include "Hazel/Core/Allocation/MemoryPool/BaseMemoryPool.h"

namespace Hazel
{
class FixedMemoryPool : public BaseMemoryPool
{
public:
    FixedMemoryPool();
    virtual ~FixedMemoryPool();

    virtual void *Allocate(const size_t allocateSize);

    template <typename T>
    void Init(const uint TotalNumber)
    {
        if (m_Allocator)
        {
            return;
        }

        size_t AllocTotalSize = (size_t)(TotalNumber * sizeof(T));

        m_Allocator = new PoolAllocator(AllocTotalSize, sizeof(T));

        // m_Alignment = MemoryPoolInfo::ALIGNMENT;

        if (m_Allocator == nullptr)
            assert(false);

        m_SingleDataSize = sizeof(T);

        m_Allocator->Init();
    }

    void Init(const uint TotalNumber, int SingleSize);

protected:
    virtual void onFree(void *ptr);

private:
    size_t m_SingleDataSize;
    PoolAllocator *m_Allocator;
};
} // namespace Hazel