#pragma once

#include <string>

namespace Hazel
{
struct HAZEL_API FileChooser
{
    static std::string OpenFile(const char *filter);
    static std::string SaveFile(const char *filter);
};
class Time
{
public:
    static float GetTime();
};
} // namespace Hazel