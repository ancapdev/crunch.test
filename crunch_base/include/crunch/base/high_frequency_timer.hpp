#ifndef CRUNCH_HIGH_FREQUENCY_TIMER_HPP
#define CRUNCH_HIGH_FREQUENCY_TIMER_HPP

#include "crunch/base/platform.hpp"

#if defined (CRUNCH_PLATFORM_WIN32)
#   include <windows.h>
#elif defined (CRUNCH_PLATFORM_LINUX)
#   include <time.h>
#else
#   error "Unsupported platform"
#endif

namespace Crunch {

class HighFrequencyTimer
{
public:
#if defined (CRUNCH_PLATFORM_WINDOWS)
    typedef LARGE_INTEGER SampleType;

    HighFrequencyTimer();

private:
    double mInvFrequency mInvFrequency;

public:
#else
    typedef timespec SampleType;
#endif

    SampleType Sample() const;

    double GetElapsedSeconds(SampleType begin, SampleType end) const;
};

}

#endif

