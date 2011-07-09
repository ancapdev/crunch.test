// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/concurrency/meta_scheduler.hpp"
#include "crunch/concurrency/processor_affinity.hpp"
#include "crunch/benchmarking/stopwatch.hpp"
#include "crunch/benchmarking/statistical_profiler.hpp"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>


// TODO: replace with result sink
#include <iostream>

namespace Crunch { namespace Concurrency {

BOOST_AUTO_TEST_SUITE(MetaSchedulerBenchmarks)

BOOST_AUTO_TEST_CASE(SingleThreadedWaitForReadyWaitableBenchmark)
{
    struct NullWaitable : IWaitable
    {
        virtual void AddWaiter(Waiter* waiter) { waiter->Notify(); }
        virtual bool RemoveWaiter(Waiter*) { return false; }
        virtual bool IsOrderDependent() const { return false; }
    };

    MetaScheduler::SchedulerList schedulers;
    MetaScheduler ms(schedulers);
    ms.Join();
    SetCurrentThreadAffinity(ProcessorAffinity(0));
    int const reps = 10000;
    NullWaitable waitable;
    Benchmarking::Stopwatch stopwatch;

    Benchmarking::StatisticalProfiler profiler(0.01, 1000, 10000, 100);
    while (!profiler.IsDone())
    {
        stopwatch.Start();
        for (int i = 0; i < reps; ++i)
            WaitFor(waitable);
        stopwatch.Stop();
        profiler.AddSample(stopwatch.GetElapsedNanoseconds() / reps);
    }

    std::cout << "Samples: " << profiler.GetNumSamples() << std::endl;
    std::cout << "Min: " << profiler.GetMin() << "ns" << std::endl;
    std::cout << "Max: " << profiler.GetMax() << "ns" << std::endl;
    std::cout << "Median: " << profiler.GetMedian() << "ns" << std::endl;
    std::cout << "Mean: " << profiler.GetMean() << "ns" << std::endl;
    std::cout << "StdDev: " << profiler.GetStdDev() << std::endl;
    std::cout << "FilteredMin: " << profiler.GetFilteredMin() << "ns" << std::endl;
    std::cout << "FilteredMax: " << profiler.GetFilteredMax() << "ns" << std::endl;
    std::cout << "FilteredMean: " << profiler.GetFilteredMean() << "ns" << std::endl;
    std::cout << "FilteredStdDev: " << profiler.GetFilteredStdDev() << std::endl;

    ms.Leave();
}

BOOST_AUTO_TEST_SUITE_END()

}}
