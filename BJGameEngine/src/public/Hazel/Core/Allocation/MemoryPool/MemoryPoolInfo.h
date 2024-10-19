#pragma once

enum class MemoryPoolType
{
    Pool,
    Stack,
    FreeList,
    Sequential // ?
};

struct MemoryPoolInfo
{
    static const int ALIGNMENT = 8;
};

enum class FreeListAllocatorPlacementPolicy
{
    FIND_FIRST,
    FIND_BEST,
    FIND_SPEED
};