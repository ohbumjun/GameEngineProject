#pragma once

#include "BaseMemoryPool.h"
#include "MemoryPoolInfo.h"

class VariantMemoryPool : public BaseMemoryPool
{
public:
    VariantMemoryPool();
    virtual ~VariantMemoryPool();

    virtual void *Allocate(const size_t allocateSize);
    void Init(uint TotalSize,
              FreeListAllocatorPlacementPolicy policy =
                  FreeListAllocatorPlacementPolicy::FIND_SPEED);

protected:
    virtual void onFree(void *ptr);

private:
    size_t m_SingleDataSize;
    class FreeListAllocator *m_Allocator;
};
