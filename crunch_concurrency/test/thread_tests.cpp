// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/concurrency/thread.hpp"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <iostream>

namespace Crunch { namespace Concurrency {

BOOST_AUTO_TEST_SUITE(ThreadTests)

BOOST_AUTO_TEST_CASE(RemoveMe)
{
    std::cout << "Waiting for thread" << std::endl;
    Thread t([] { std::cout << "In thread" << std::endl; });
    t.Join();
    std::cout << "Joined thread" << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()

}}
