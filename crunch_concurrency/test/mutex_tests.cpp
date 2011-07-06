// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/concurrency/mutex.hpp"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

namespace Crunch { namespace Concurrency {

BOOST_AUTO_TEST_SUITE(MutexTests)

BOOST_AUTO_TEST_CASE(ConstructTest)
{
    Mutex m;
    BOOST_CHECK(!m.IsLocked());
}

BOOST_AUTO_TEST_CASE(AddWaiterToUnlockedTest)
{
    Mutex m;
    bool acquired = false;
    auto waiter = MakeWaiter([&] { acquired = true; });
    m.AddWaiter(&waiter);
    BOOST_CHECK(acquired);
    BOOST_CHECK(m.IsLocked());
    m.Release();
    BOOST_CHECK(!m.IsLocked());
}

BOOST_AUTO_TEST_CASE(AddWaiterToLockedTest)
{
    Mutex m;
    bool acquired1 = false;
    bool acquired2 = false;
    bool acquired3 = false;
    auto waiter1 = MakeWaiter([&] { acquired1 = true; });
    auto waiter2 = MakeWaiter([&] { acquired2 = true; });
    auto waiter3 = MakeWaiter([&] { acquired3 = true; });
    m.AddWaiter(&waiter1);
    m.AddWaiter(&waiter2);
    m.AddWaiter(&waiter3);
    BOOST_CHECK(acquired1);
    BOOST_CHECK(!acquired2);
    BOOST_CHECK(!acquired3);
    BOOST_CHECK(m.IsLocked());
    m.Release();
    BOOST_CHECK(!acquired2);
    BOOST_CHECK(acquired3);
    BOOST_CHECK(m.IsLocked());
    m.Release();
    BOOST_CHECK(acquired2);
    BOOST_CHECK(m.IsLocked());
    m.Release();
    BOOST_CHECK(!m.IsLocked());
}

BOOST_AUTO_TEST_CASE(RemoveWaiterTest)
{
    Mutex m;
    bool acquired1 = false;
    bool acquired2 = false;
    auto waiter1 = MakeWaiter([&] { acquired1 = true; });
    auto waiter2 = MakeWaiter([&] { acquired2 = true; });
    m.AddWaiter(&waiter1);
    m.AddWaiter(&waiter2);
    BOOST_CHECK(acquired1);
    BOOST_CHECK(!acquired2);
    BOOST_CHECK(m.IsLocked());
    m.RemoveWaiter(&waiter2);
    m.Release();
    BOOST_CHECK(!acquired2);
    BOOST_CHECK(!m.IsLocked());
}

BOOST_AUTO_TEST_SUITE_END()

}}
