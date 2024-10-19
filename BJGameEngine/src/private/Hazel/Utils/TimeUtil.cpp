#include "Hazel/Utils/TimeUtil.h"

#pragma comment(lib, "Winmm.lib")

namespace Hazel
{
static uint32 start = 0;
static bool ticksStarted = false;

// 고해상도 Tick
static bool hiresTimerAvailable;
static LARGE_INTEGER hiresStartTicks;
static LARGE_INTEGER hiresTicksPerSecond;

void TimeUtil::InitTicks(void)
{
    if (ticksStarted)
    {
        return;
    }
    ticksStarted = true;

    if (QueryPerformanceFrequency(&hiresTicksPerSecond) == true)
    {
        hiresTimerAvailable = true;
        QueryPerformanceCounter(&hiresStartTicks);
    }
    else
    {
        hiresTimerAvailable = false;
#ifndef __WINRT__
        start = timeGetTime();
#endif
    }
}

uint32 TimeUtil::GetTimeMiliSec(void)
{
    uint32 now = 0;
    LARGE_INTEGER hires_now;

    if (!ticksStarted)
    {
        InitTicks();
    }

    if (hiresTimerAvailable)
    {
        QueryPerformanceCounter(&hires_now);

        return (uint32)((hires_now.QuadPart - hiresStartTicks.QuadPart) /
                        (hiresTicksPerSecond.QuadPart / 1000));
    }
    else
    {
#ifndef __WINRT__
        now = timeGetTime();
#endif
    }

    return (now - start);
}

size_t TimeUtil::GetTimeMicroSec(void)
{
    size_t now = 0;
    LARGE_INTEGER hires_now;

    if (!ticksStarted)
        InitTicks();

    if (hiresTimerAvailable)
    {
        QueryPerformanceCounter(&hires_now);

        hires_now.QuadPart -= hiresStartTicks.QuadPart;
        hires_now.QuadPart /= (hiresTicksPerSecond.QuadPart / 1000000);

        return (size_t)hires_now.QuadPart;
    }
    else
    {
#ifndef __WINRT__
        now = timeGetTime();
#endif
    }

    return (now - start);
}
int64 TimeUtil::GetTimeDateNow(void)
{
    return time(NULL);
}
}