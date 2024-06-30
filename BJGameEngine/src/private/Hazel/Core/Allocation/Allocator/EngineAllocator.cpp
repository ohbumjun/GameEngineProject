#include "Hazel/Core/Allocation/Allocator/EngineAllocator.h"
#include "Hazel/Core/Allocation/Allocator/FreeListAllocator.h"
#include "Hazel/Core/Allocation/Allocator/PoolAllocator.h"
#include "hzpch.h"

namespace Hazel
{
static FreeListAllocator s_FreeListAllocator(10000000); // 10mb
static EngineAllocator *s_EngineAllocator = nullptr;

EngineAllocator *BJ_GetEngineAllocator()
{
    return s_EngineAllocator;
}
void *BJ_EngineAllocator(size_t size)
{
    // size 에 맞춰서, FreeListAllocator 를 사용할지 PoolAllocator 를 사용할지 결정
    return s_FreeListAllocator.Allocate(size);
}
void BJ_EngineFree(void *ptr)
{
    s_FreeListAllocator.Free(ptr);
}
} // namespace Hazel