#pragma once

// Standard
#include <assert.h>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

// Utils
#include <algorithm>
#include <functional>
#include <memory>
#include <string_view>
#include <type_traits>
#include <utility>

// Platform
#include <process.h>

// DS
#include <array>
#include <deque>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Hazel/Debug/Instrumentor.h"
#include "Hazel/Utils/Log.h"

#ifdef HZ_PLATFORM_WINDOWS
#include <Windows.h>
#endif

// Types
// typedef char int8;
typedef signed char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long uint64;

// Exception
#define THROW(fmt, ...)                                                        \
    do                                                                         \
    {                                                                          \
        fprintf(stderr, fmt "\n", ##__VA_ARGS__);                              \
        __debugbreak();                                                        \
    } while (false)


#define ALIGNOF(...) __alignof(__VA_ARGS__)