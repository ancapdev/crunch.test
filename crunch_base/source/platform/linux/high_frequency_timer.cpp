#include "crunch/base/high_frequency_timer.hpp"

namespace Crunch {

HighFrequencyTimer::SampleType HighFrequencyTimer::Sample() const
{
    timespec sample;
    clock_gettime(CLOCK_MONOTONIC, &sample);
    return sample;
}

double HighFrequencyTimer::GetElapsedSeconds(SampleType begin, SampleType end) const
{
    return difftime(end.tv_sec, begin.tv_sec) + (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
}

}
