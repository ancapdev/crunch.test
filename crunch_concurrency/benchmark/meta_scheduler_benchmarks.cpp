// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/concurrency/meta_scheduler.hpp"
#include "crunch/concurrency/processor_affinity.hpp"
#include "crunch/benchmarking/stopwatch.hpp"
#include "crunch/benchmarking/statistical_profiler.hpp"
#include "crunch/benchmarking/result_table.hpp"
#include "crunch/benchmarking/stream_result_sink.hpp"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <array>
#include <tuple>

namespace Crunch { namespace Concurrency {

struct NullWaitable : IWaitable
{
    virtual void AddWaiter(Waiter* waiter) { waiter->Notify(); }
    virtual bool RemoveWaiter(Waiter*) { return false; }
    virtual bool IsOrderDependent() const { return false; }
};

BOOST_AUTO_TEST_SUITE(MetaSchedulerBenchmarks)

BOOST_AUTO_TEST_CASE(SingleThreadedWaitForReadyWaitableBenchmark)
{
    using namespace Benchmarking;

    MetaScheduler::SchedulerList schedulers;
    MetaScheduler ms(schedulers);
    MetaScheduler::Context& context = ms.AcquireContext();
    SetCurrentThreadAffinity(ProcessorAffinity(0));
    int const reps = 10000;
    NullWaitable waitable;
    Stopwatch stopwatch;

    ResultTable<std::tuple<char const*, char const*, double, double, double, double, double>> results(
        "WaitForReady",
        1,
        std::make_tuple("call_mode", "wait_mode", "min", "max", "mean", "median", "stddev"));

    typedef std::tuple<char const*, WaitMode> NamedWaitMode;
    std::array<NamedWaitMode, 3> const waitModes =
    {
        std::make_tuple("poll", WaitMode::Poll()),
        std::make_tuple("block", WaitMode::Block()),
        std::make_tuple("run", WaitMode::Run())
    };

    StatisticalProfiler profiler(0.01, 1000, 10000, 100);

    std::for_each(waitModes.begin(), waitModes.end(), [&] (NamedWaitMode const& waitMode)
    {
        for (int direct = 0; direct != 2; ++direct)
        {
            profiler.Reset();
            while (!profiler.IsDone())
            {
                stopwatch.Start();
                if (direct == 0)
                {
                    for (int i = 0; i < reps; ++i)
                        WaitFor(waitable, std::get<1>(waitMode));
                }
                else
                {
                    for (int i = 0; i < reps; ++i)
                        context.WaitFor(waitable, std::get<1>(waitMode));
                }
                stopwatch.Stop();
                profiler.AddSample(stopwatch.GetElapsedNanoseconds() / reps);
            }

            results.Add(std::make_tuple(
                direct == 0 ? "global" : "context",
                std::get<0>(waitMode),
                profiler.GetMin(),
                profiler.GetMax(),
                profiler.GetMean(),
                profiler.GetMedian(),
                profiler.GetStdDev()));
        }
    });

    context.Release();
}

BOOST_AUTO_TEST_SUITE_END()

}}
