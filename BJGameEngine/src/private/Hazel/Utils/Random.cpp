#include "hzpch.h"
#include "Hazel/Utils/Random.h"

HAZEL_API std::mt19937 Random::s_RandomEngine;
HAZEL_API std::uniform_int_distribution<std::mt19937::result_type>
    Random::s_Distribution;