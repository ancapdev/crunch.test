#include "crunch/base/high_frequency_timer.hpp"

#include <windows.h>

namespace Crunch {

HighFrequencyTimer::HighFrequencyTimer()
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    mInvFrequency = 1.0 / static_cast<double>(frequency.QuadPart);
}

HighFrequencyTimer::SampleType HighFrequencyTimer::Sample() const
{
    LARGE_INTEGER sample;
    QueryPerformanceCounter(&sample);
    return sample.QuadPart;
}

double HighFrequencyTimer::GetElapsedSeconds(SampleType begin, SampleType end) const
{
    return static_cast<double>(end - begin) * mInvFrequency;
}

}
