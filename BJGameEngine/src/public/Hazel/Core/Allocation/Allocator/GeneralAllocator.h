#pragma once

#include "hzpch.h"
#include "Allocable.h"
#include "Hazel/Core/Memory/CustomMemory.h"

namespace Hazel
{

// Align 을 맞춰서 할당해주는 Allocator 
template <typename T>
struct AlignedAllocator final : public Allocable
{
    size_t m_Align = 4;

    explicit AlignedAllocator()
    {
        m_Align = ALIGNOF(T);
    }

    AlignedAllocator(size_t align) : m_Align(align)
    {
    }

    AlignedAllocator(const AlignedAllocator &allocator)
    {
        m_Align = allocator.m_Align;
    }

    AlignedAllocator &operator=(const AlignedAllocator &o)
    {
        return *this;
    }

    ~AlignedAllocator(){};

    void *Allocate(size_t size,
                   const char *file = nullptr,
                   size_t line = 0) override
    {
        return MemoryUtil::HazelAlignedAlloc(size, m_Align);
    }

    void *Reallocate(void *ptr,
                     size_t size,
                     const char *file = nullptr,
                     size_t line = 0) override
    {
        return MemoryUtil::HazelAlignedRealloc(ptr, m_Align, size);
    }

    void Free(void *ptr) override
    {
        MemoryUtil::HazelAlignedFree(ptr);
    }
};
} // namespace Hazel