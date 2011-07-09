// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/concurrency/meta_scheduler.hpp"
#include "crunch/base/high_frequency_timer.hpp"

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
    int const reps = 100000000;
    NullWaitable waitable;
    HighFrequencyTimer timer;
    HighFrequencyTimer::SampleType start = timer.Sample();

    for (int i = 0; i < reps; ++i)
    {
        WaitFor(waitable);
    }

    HighFrequencyTimer::SampleType end = timer.Sample();
    std::cout << timer.GetElapsedNanoseconds(start, end) / reps << "ns" << std::endl;

    ms.Leave();
}

BOOST_AUTO_TEST_SUITE_END()

}}
