#pragma once


// Standard
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <bitset>
#include <string>
#include <sstream>
#include <iostream>
#include <cstdint>

// Utils
#include <memory>
#include <algorithm>
#include <type_traits>
#include <functional>
#include <utility>
#include <string_view>

// Platform
#include <process.h>

// DS
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <deque>
#include <queue>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <tuple>

#include "Hazel/Utils/Log.h"
#include "Hazel/Debug/Instrumentor.h"

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
#define THROW(fmt, ...) do { fprintf(stderr, fmt "\n", ##__VA_ARGS__); __debugbreak(); } while (false)