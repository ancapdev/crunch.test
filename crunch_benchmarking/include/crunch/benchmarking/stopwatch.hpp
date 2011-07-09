// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_BENCHMARKING_STOPWATCH_HPP
#define CRUNCH_BENCHMARKING_STOPWATCH_HPP

#include "crunch/base/high_frequency_timer.hpp"

namespace Crunch { namespace Benchmarking {

class Stopwatch
{
public:
    Stopwatch();

    void Stopwatch::Start();
    void Stopwatch::Stop();

    double Stopwatch::GetElapsedSeconds() const;
    double Stopwatch::GetElapsedMilliseconds() const;
    double Stopwatch::GetElapsedMicroseconds() const;
    double Stopwatch::GetElapsedNanoseconds() const;

private:
    HighFrequencyTimer mTimer;
    HighFrequencyTimer::SampleType mStart;
    HighFrequencyTimer::SampleType mStop;
};

inline Stopwatch::Stopwatch()
    : mStart(mTimer.Sample())
    , mStop(mTimer.Sample())
{}

inline void Stopwatch::Start()
{
    mStart = mTimer.Sample();
}

inline void Stopwatch::Stop()
{
    mStop = mTimer.Sample();
}

inline double Stopwatch::GetElapsedSeconds() const
{
    return mTimer.GetElapsedSeconds(mStart, mStop);
}

inline double Stopwatch::GetElapsedMilliseconds() const
{
    return mTimer.GetElapsedMilliseconds(mStart, mStop);
}

inline double Stopwatch::GetElapsedMicroseconds() const
{
    return mTimer.GetElapsedMicroseconds(mStart, mStop);
}

inline double Stopwatch::GetElapsedNanoseconds() const
{
    return mTimer.GetElapsedNanoseconds(mStart, mStop);
}

}}

#endif
