// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/benchmarking/statistical_profiler.hpp"

#include <algorithm>
#include <numeric>

#include <iostream>

namespace Crunch { namespace Benchmarking {

StatisticalProfiler::StatisticalProfiler(double targetNormStdDev, std::size_t minSamples, std::size_t maxSamples, std::size_t samplesPerCalculation)
    : mTargetStdDev(targetNormStdDev)
    , mMinSamples(minSamples)
    , mMaxSamples(maxSamples)
    , mSamplesPerCalculation(samplesPerCalculation)
{
    Reset();
}

void StatisticalProfiler::Reset()
{
    mSamples.clear();
    mLastCalculationSize = 0;
    mMin = 0.0;
    mMax = 0.0;
    mMean = 0.0;
    mMedian = 0.0;
    mStdDev = 0.0;
    mFilteredMin = 0.0;
    mFilteredMax = 0.0;
    mFilteredMean = 0.0;
    mFilteredStdDev = 0.0;
}

bool StatisticalProfiler::IsDone() const
{
    if (mSamples.size() >= mLastCalculationSize + mSamplesPerCalculation)
        UpdateStatistics();

    if (mSamples.size() >= mMaxSamples)
        return true;

    if (mSamples.size() < mMinSamples)
        return false;

    return mFilteredStdDev <= mTargetStdDev * mFilteredMean;
}

double StatisticalProfiler::GetMin() const
{
    UpdateStatistics();
    return mMin;
}

double StatisticalProfiler::GetMax() const
{
    UpdateStatistics();
    return mMax;
}

double StatisticalProfiler::GetMean() const
{
    UpdateStatistics();
    return mMean;
}

double StatisticalProfiler::GetMedian() const
{
    UpdateStatistics();
    return mMedian;
}

double StatisticalProfiler::GetStdDev() const
{
    UpdateStatistics();
    return mStdDev;
}

double StatisticalProfiler::GetFilteredMin() const
{
    UpdateStatistics();
    return mFilteredMin;
}

double StatisticalProfiler::GetFilteredMax() const
{
    UpdateStatistics();
    return mFilteredMax;
}

double StatisticalProfiler::GetFilteredMean() const
{
    UpdateStatistics();
    return mFilteredMean;
}

double StatisticalProfiler::GetFilteredStdDev() const
{
    UpdateStatistics();
    return mFilteredStdDev;
}

void StatisticalProfiler::UpdateStatistics() const
{
    if (mSamples.size() == mLastCalculationSize)
        return;

    mLastCalculationSize = mSamples.size();

    std::sort(mSamples.begin(), mSamples.end());

    mMin = mSamples[0];
    mMax = mSamples[mSamples.size() - 1];
    mMean = std::accumulate(mSamples.begin(), mSamples.end(), 0.0) / static_cast<double>(mSamples.size());
    if ((mSamples.size() % 2) == 0)
        mMedian = (mSamples[mSamples.size() / 2] + mSamples[(mSamples.size() + 1) / 2]) / 2.0;
    else
        mMedian = mSamples[mSamples.size() / 2];

    mStdDev = std::accumulate(mSamples.begin(), mSamples.end(), 0.0, [&] (double acc, double x) { return acc + (x-mMean)*(x-mMean); }) / static_cast<double>(mSamples.size());

    std::size_t const q1 = mSamples.size() / 4;
    std::size_t const q3 = mSamples.size() - q1;

    mFilteredMin = mSamples[q1];
    mFilteredMax = mSamples[q3 - 1];
    mFilteredMean = std::accumulate(mSamples.begin() + q1, mSamples.begin() + q3, 0.0) / static_cast<double>(q3 - q1);
    mFilteredStdDev = std::accumulate(mSamples.begin() + q1, mSamples.begin() + q3, 0.0, [&] (double acc, double x) { return acc + (x-mFilteredMean)*(x-mFilteredMean); }) / static_cast<double>(q3 - q1);
}

}}
