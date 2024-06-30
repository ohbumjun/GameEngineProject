#pragma once

#include "MemoryPoolAllocator.h"
#include "Hazel/Core/Allocation/Allocator/Allocable.h"
#include <cstddef>

namespace Hazel
{
	
class EngineAllocator 
{
};

void* BJ_EngineAllocate(size_t size);
void BJ_EngineFree(void* ptr);
} // namespace Hazel
