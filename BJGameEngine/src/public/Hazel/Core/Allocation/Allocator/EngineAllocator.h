#pragma once

#include "MemoryPoolAllocator.h"
#include "Hazel/Core/Allocation/Allocator/Allocable.h"
#include <cstddef>

namespace Hazel
{
	
class EngineAllocator 
{
public:
    static void *BJ_EngineAllocate(size_t size);
    static void BJ_EngineFree(void *ptr);
};
EngineAllocator *BJ_GetEngineAllocator();

} // namespace Hazel
