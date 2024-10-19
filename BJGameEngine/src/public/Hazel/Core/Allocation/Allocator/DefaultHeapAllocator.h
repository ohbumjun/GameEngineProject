#pragma once

#include "Allocable.h"

namespace Hazel
{
struct HeapAllocator : Allocable
{
    void *Allocate(size_t size,
                   const char *file = nullptr,
                   size_t line = 0) override;

    template <typename T>
    void *Allocate(const char *file = nullptr, const size_t line = 0)
    {
        return Allocate(sizeof(T), file, line);
    }

    void *Reallocate(void *ptr,
                    size_t size,
                    const char *file = nullptr,
                    size_t line = 0) override;

    template <typename T>
    void *Reallocate(void *ptr, const char *file = nullptr, const size_t line = 0)
    {
        return Realloc(ptr, sizeof(T), file, line);
    }

    void Free(void *ptr);
};

} // namespace Hazel
