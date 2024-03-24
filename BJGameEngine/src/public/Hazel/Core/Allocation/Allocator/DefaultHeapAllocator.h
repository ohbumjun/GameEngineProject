#pragma once

#include "Allocable.h"

namespace Hazel
{
struct DefaultHeapAllocator : Allocable
{
    void *Allocate(size_t size,
                   const char *file = nullptr,
                   size_t line = 0) override;

    template <typename T>
    void *Allocate(const char *file = nullptr, const size_t line = 0)
    {
        return Allocate(sizeof(T), file, line);
    }

    // template<typename T, typename... TArgs>
    // T* New(const char* file, const size_t line, TArgs... args)
    // {
    // 	return new(Alloc<T>(file, line))T(args...);
    // }

    void *Reallocate(void *ptr,
                    size_t size,
                    const char *file = nullptr,
                    size_t line = 0) override;

    template <typename T>
    void *Realloc(void *ptr, const char *file = nullptr, const size_t line = 0)
    {
        return Realloc(ptr, sizeof(T), file, line);
    }


    void Free(void *ptr);
};

} // namespace Hazel
