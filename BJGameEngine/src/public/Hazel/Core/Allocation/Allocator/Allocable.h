#pragma once

#include "Memory.h"
namespace Hazel
{

class Allocable
{
public:
    // file, line : memory leak tracking �� �����̴�.
    virtual void *Allocate(size_t size,
                           const char *flie = nullptr,
                           size_t line = 0) = 0;

    virtual void *Reallocate(void *ptr,
                             size_t size,
                             const char *flie = nullptr,
                             size_t line = 0) = 0;

    virtual void Free(void *ptr) = 0;
};
} // namespace Hazel