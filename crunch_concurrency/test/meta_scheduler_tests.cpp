// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/concurrency/meta_scheduler.hpp"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

namespace Crunch { namespace Concurrency {

BOOST_AUTO_TEST_SUITE(MetaSchedulerTests)

BOOST_AUTO_TEST_CASE(WaitTest)
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

    NullWaitable nullWaitable;
    IWaitable* waitables[] = { &nullWaitable };

    WaitFor(nullWaitable, WaitMode::Block());
    WaitForAll(waitables, 1, WaitMode::Block());
    WaitForAny(waitables, 1, WaitMode::Block());

    ms.Leave();
}


BOOST_AUTO_TEST_SUITE_END()

}}
