// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/base/memory.hpp"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

namespace Crunch {

BOOST_AUTO_TEST_SUITE(MemoryTests)

BOOST_AUTO_TEST_CASE(MallocAlignedTest)
{
    for (std::size_t alignment = 1; alignment < 1024; alignment *= 2)
    {
        void* p = MallocAligned(1, alignment);
        BOOST_CHECK(IsPointerAligned(p, alignment));
        FreeAligned(p);
    }
}

BOOST_AUTO_TEST_CASE(AlignPointerUpTest)
{
    BOOST_CHECK_EQUAL(AlignPointerUp((void*)0, 16), (void*)0);
    BOOST_CHECK_EQUAL(AlignPointerUp((void*)1, 16), (void*)16);
}

BOOST_AUTO_TEST_CASE(AlignPointerDownTest)
{
    BOOST_CHECK_EQUAL(AlignPointerDown((void*)0, 16), (void*)0);
    BOOST_CHECK_EQUAL(AlignPointerDown((void*)1, 16), (void*)0);
}

BOOST_AUTO_TEST_CASE(IsPointerAlignedTest)
{
    BOOST_CHECK(IsPointerAligned((void*)0, 1024));
    BOOST_CHECK(IsPointerAligned((void*)1, 1));
    BOOST_CHECK(!IsPointerAligned((void*)1, 2));
}

BOOST_AUTO_TEST_CASE(OffsetPointerTest)
{
    BOOST_CHECK_EQUAL(OffsetPointer((void*)100, 1), (void*)101);
    BOOST_CHECK_EQUAL(OffsetPointer((void*)100, -1), (void*)99);
}

BOOST_AUTO_TEST_SUITE_END()

}
