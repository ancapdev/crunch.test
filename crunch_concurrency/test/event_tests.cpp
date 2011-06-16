#include "crunch/concurrency/event.hpp"
#include "crunch/base/override.hpp"
#include "crunch/base/stdint.hpp"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

namespace Crunch { namespace Concurrency {

struct TestWaiter : Waiter
{
    TestWaiter() : wakeupCount(0) { }

    virtual void Wakeup() CRUNCH_OVERRIDE
    {
        wakeupCount++;
    }

    uint32 wakeupCount;
};

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

    e.Clear();
    BOOST_CHECK(!e.IsSet());

    e.Set();
    BOOST_CHECK(e.IsSet());
}

BOOST_AUTO_TEST_CASE(AddWaiterToSetTest)
{
    Event e(true);
    TestWaiter waiter;

    // Add waiter to set event. Should be called immediately.
    e.AddWaiter(&waiter);
    BOOST_CHECK_EQUAL(waiter.wakeupCount, 1);
}

BOOST_AUTO_TEST_CASE(AddWaiterToUnsetTest)
{
    Event e(false);
    TestWaiter waiter;

    // Add waiter to unset event.
    e.AddWaiter(&waiter);
    BOOST_CHECK_EQUAL(waiter.wakeupCount, 0);

    // Remove waiter and set event.
    e.RemoveWaiter(&waiter);
    e.Set();
    BOOST_CHECK_EQUAL(waiter.wakeupCount, 0);

    // Add waiter and set event.
    e.AddWaiter(&waiter);
    e.Set();
    BOOST_CHECK_EQUAL(waiter.wakeupCount, 1);
}

BOOST_AUTO_TEST_SUITE_END()

}}
