#pragma once

#include "hzpch.h"
#include "Allocable.h"
#include "Hazel/Core/Memory/CustomMemory.h"

// #if defined(__WIN32__) || defined(__ANDROID__)
// #include <malloc.h>
// #else
namespace Hazel
{

template <typename T>
class GeneralAllocator : Allocable
{
    // T Type 의 객체를 여러 개 할당하는 함수
    inline void *Allocate(size_t count,
                          const char *file = nullptr,
                          size_t line = 0)
    {
        return Allocate(sizeof(T) * count, file, line);
    }

    inline void *Reallocate(T *ptr,
                            size_t count,
                            const char *flie = nullptr,
                            size_t line = 0)
    {
        return Reallocate(ptr, sizeof(T) * count, flie, line);
    }
};

// Align 을 맞춰서 할당해주는 Allocator //
template <typename T>
struct AlignedAllocator final : public GeneralAllocator<T>
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
        // return static_cast<T*>(pr_aligned_alloc(size, m_Align));
        return MemoryUtil::HazelAlignedAlloc(size, m_Align);
    }

    void *Reallocate(void *ptr,
                     size_t size,
                     const char *file = nullptr,
                     size_t line = 0) override
    {
        // return static_cast<T*>(pr_aligned_realloc(ptr, m_Align, size));
        return MemoryUtil::HazelAlignedRealloc(ptr, m_Align, size);
    }

    void Free(void *ptr) override
    {
        MemoryUtil::HazelAlignedFree(ptr);
    }
};
} // namespace Hazel