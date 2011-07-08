// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_HIGH_FREQUENCY_TIMER_HPP
#define CRUNCH_HIGH_FREQUENCY_TIMER_HPP

#include "crunch/base/platform.hpp"

#if defined (CRUNCH_PLATFORM_WIN32)
#elif defined (CRUNCH_PLATFORM_LINUX)
#   include <time.h>
#else
#   error "Unsupported platform"
#endif

namespace Crunch {

class HighFrequencyTimer
{
public:
#if defined (CRUNCH_PLATFORM_WIN32)
    typedef __int64 SampleType;
    HighFrequencyTimer();
#else
    typedef timespec SampleType;
#endif

    SampleType Sample() const;
    double GetElapsedSeconds(SampleType begin, SampleType end) const;
    double GetElapsedMilliseconds(SampleType begin, SampleType end) const;
    double GetElapsedMicroseconds(SampleType begin, SampleType end) const;
    double GetElapsedNanoseconds(SampleType begin, SampleType end) const;

private:
#if defined (CRUNCH_PLATFORM_WIN32)
    double mInvFrequency;
#endif
};

inline double HighFrequencyTimer::GetElapsedMilliseconds(SampleType begin, SampleType end) const
{
    return GetElapsedSeconds(begin, end) * 1000.0;
}

inline double HighFrequencyTimer::GetElapsedMicroseconds(SampleType begin, SampleType end) const
{
    return GetElapsedSeconds(begin, end) * 1000000.0;
}

inline double HighFrequencyTimer::GetElapsedNanoseconds(SampleType begin, SampleType end) const
{
    return GetElapsedSeconds(begin, end) * 1000000000.0;
}

}

#endif

