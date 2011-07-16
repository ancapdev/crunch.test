// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/concurrency/event.hpp"
#include "crunch/base/override.hpp"
#include "crunch/base/stdint.hpp"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

namespace Crunch { namespace Concurrency {

BOOST_AUTO_TEST_SUITE(EventTests)

BOOST_AUTO_TEST_CASE(ConstructSetTest)
{
    Event e(true);
    BOOST_CHECK(e.IsSet());
    BOOST_CHECK(!e.IsOrderDependent());
}

BOOST_AUTO_TEST_CASE(ConstructUnsetTest)
{
    Event e(false);
    BOOST_CHECK(!e.IsSet());
    BOOST_CHECK(!e.IsOrderDependent());
}

BOOST_AUTO_TEST_CASE(StateChangeTest)
{
    Event e(true);
    BOOST_CHECK(e.IsSet());

    e.Reset();
    BOOST_CHECK(!e.IsSet());

    e.Set();
    BOOST_CHECK(e.IsSet());
}

BOOST_AUTO_TEST_CASE(AddWaiterToSetTest)
{
    Event e(true);
    volatile uint32 wakeupCount = 0;

    // Add waiter to set event. Should be called immediately.
    e.AddWaiter([&] { wakeupCount++; });
    BOOST_CHECK_EQUAL(wakeupCount, 1u);
}

BOOST_AUTO_TEST_CASE(AddWaiterToUnsetTest)
{
    Event e(false);
    volatile uint32 wakeupCount = 0;

    struct CountIncrementer
    {
        static void Increment(void* count)
        {
            (*reinterpret_cast<uint32*>(count))++;
        }
    };
    DestroyableWaiter* waiter = DestroyableWaiter::Create(&CountIncrementer::Increment, &wakeupCount);

    // Add waiter to unset event.
    e.AddWaiter(waiter);
    BOOST_CHECK_EQUAL(wakeupCount, 0u);

    // Remove waiter and set event.
    e.RemoveWaiter(waiter);
    e.Set();
    BOOST_CHECK_EQUAL(wakeupCount, 0u);

    // Add waiter and set event.
    e.AddWaiter(waiter);
    e.Set();
    BOOST_CHECK_EQUAL(wakeupCount, 1u);

    waiter->Destroy();
}

BOOST_AUTO_TEST_SUITE_END()

}}
