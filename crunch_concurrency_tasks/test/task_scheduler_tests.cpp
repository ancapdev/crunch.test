// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/concurrency/task_scheduler.hpp"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <windows.h>
namespace Crunch { namespace Concurrency {

BOOST_AUTO_TEST_SUITE(TaskSchedulerTests)

BOOST_AUTO_TEST_CASE(RemoveMe)
{
    TaskScheduler scheduler;
    Future<int> result = scheduler.Add([]{ return 1; }, nullptr, 0);
    scheduler.RunAll();
    BOOST_CHECK_EQUAL(result.Get(), 1);

}

BOOST_AUTO_TEST_SUITE_END()

}}