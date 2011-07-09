// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_BENCHMARKING_STATISTICAL_PROFILER_HPP
#define CRUNCH_BENCHMARKING_STATISTICAL_PROFILER_HPP

#include <vector>

namespace Crunch { namespace Benchmarking {

class StatisticalProfiler
{
public:
    StatisticalProfiler(double targetNormStdDev, std::size_t minSamples, std::size_t maxSamples, std::size_t samplesPerCalculation = 1);

    void AddSample(double sample);

    void Reset();

    bool IsDone() const;

    std::size_t GetNumSamples() const;
    double GetMin() const;
    double GetMax() const;
    double GetMean() const;
    double GetMedian() const;
    double GetStdDev() const;
    double GetRawMin() const;
    double GetRawMax() const;
    double GetRawMean() const;
    double GetRawStdDev() const;

private:
    void UpdateStatistics() const;

    double mTargetStdDev;
    std::size_t mMinSamples;
    std::size_t mMaxSamples;
    std::size_t mSamplesPerCalculation;
    mutable std::vector<double> mSamples;
    mutable std::size_t mLastCalculationSize;
    mutable double mRawMin;
    mutable double mRawMax;
    mutable double mRawMean;
    mutable double mRawStdDev;
    mutable double mMin;
    mutable double mMax;
    mutable double mMean;
    mutable double mMedian;
    mutable double mStdDev;
};

inline void StatisticalProfiler::AddSample(double sample)
{
    mSamples.push_back(sample);
}

inline std::size_t StatisticalProfiler::GetNumSamples() const
{
    return mSamples.size();
}

}}

#endif
