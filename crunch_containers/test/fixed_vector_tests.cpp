// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/containers/fixed_vector.hpp"
#include "./tracker.hpp"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <stdexcept>

namespace Crunch { namespace Containers {

BOOST_AUTO_TEST_SUITE(FixedVectorTests)

BOOST_AUTO_TEST_CASE(DefaultConstructTest)
{
    FixedVector<Tracker, 8> const v;
    BOOST_CHECK(v.empty());
    BOOST_CHECK_EQUAL(v.begin(), v.end());
    BOOST_CHECK_EQUAL(&v[0], v.begin());
    BOOST_CHECK_EQUAL(v.size(), 0u);
    BOOST_CHECK_EQUAL(v.max_size(), 8u);
    BOOST_CHECK_EQUAL(v.capacity(), 8u);
}

BOOST_AUTO_TEST_CASE(FillConstructTest)
{
    Tracker::Statistics stats;
    {
        FixedVector<Tracker, 8> const v(3, Tracker(stats, 123));
        BOOST_CHECK_EQUAL(v.size(), 3u);
        BOOST_CHECK_EQUAL(v[0].GetTag(), 123);
        BOOST_CHECK_EQUAL(v[1].GetTag(), 123);
        BOOST_CHECK_EQUAL(v[2].GetTag(), 123);
    }
    BOOST_CHECK_EQUAL(stats.constructCount, stats.destructCount);
}

BOOST_AUTO_TEST_CASE(RangeConstructTest)
{
    Tracker::Statistics stats;
    {
        Tracker trackers[3] =
        {
            Tracker(stats, 1),
            Tracker(stats, 2),
            Tracker(stats, 3)
        };
        FixedVector<Tracker, 8> const v(trackers, trackers + 3);
        BOOST_CHECK_EQUAL(v.size(), 3u);
        BOOST_CHECK_EQUAL(v[0].GetTag(), 1);
        BOOST_CHECK_EQUAL(v[1].GetTag(), 2);
        BOOST_CHECK_EQUAL(v[2].GetTag(), 3);
    }
    BOOST_CHECK_EQUAL(stats.constructCount, stats.destructCount);
}

BOOST_AUTO_TEST_CASE(CopyConstructTest)
{
    Tracker::Statistics stats;
    {
        FixedVector<Tracker, 8> const v(3, Tracker(stats, 123));
        FixedVector<Tracker, 8> const v2(v);
        BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.end(), v2.begin(), v2.end());
    }
    BOOST_CHECK_EQUAL(stats.constructCount, stats.destructCount);
}

BOOST_AUTO_TEST_CASE(MoveConstructTest)
{
    Tracker::Statistics stats;
    {
        FixedVector<Tracker, 8> v(3, Tracker(stats, 123));
        FixedVector<Tracker, 8> const v2(std::move(v));
        BOOST_CHECK(v.empty());
        BOOST_CHECK_EQUAL(v2.size(), 3);
        BOOST_CHECK_EQUAL(v2[0].GetTag(), 123);
        BOOST_CHECK_EQUAL(v2[1].GetTag(), 123);
        BOOST_CHECK_EQUAL(v2[2].GetTag(), 123);
    }
    BOOST_CHECK_EQUAL(stats.constructCount, stats.destructCount);
}

BOOST_AUTO_TEST_CASE(CopyAssignTest)
{
    Tracker::Statistics stats;
    {
        FixedVector<Tracker, 8> const v(3, Tracker(stats, 123));
        FixedVector<Tracker, 8> v2;
        v2 = v;
        BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.end(), v2.begin(), v2.end());
    }
    BOOST_CHECK_EQUAL(stats.constructCount, stats.destructCount);

}

BOOST_AUTO_TEST_CASE(MoveAssignTest)
{
    Tracker::Statistics stats;
    {
        FixedVector<Tracker, 8> v(3, Tracker(stats, 123));
        FixedVector<Tracker, 8> v2;
        v2 = std::move(v);
        BOOST_CHECK(v.empty());
        BOOST_CHECK_EQUAL(v2.size(), 3);
        BOOST_CHECK_EQUAL(v2[0].GetTag(), 123);
        BOOST_CHECK_EQUAL(v2[1].GetTag(), 123);
        BOOST_CHECK_EQUAL(v2[2].GetTag(), 123);
    }
    BOOST_CHECK_EQUAL(stats.constructCount, stats.destructCount);
}

BOOST_AUTO_TEST_CASE(PushPopTest)
{
    Tracker::Statistics stats;
    FixedVector<Tracker, 1> v;
    v.push_back(Tracker(stats));
    BOOST_CHECK_EQUAL(stats.constructCount, stats.destructCount + 1);
    BOOST_CHECK(!v.empty());
    BOOST_CHECK_EQUAL(v.size(), 1u);
    BOOST_CHECK_EQUAL(v.end(), v.begin() + 1);
    BOOST_CHECK_THROW(v.push_back(Tracker(stats)), std::length_error);
    BOOST_CHECK_EQUAL(v.size(), 1u);
    v.pop_back();
    BOOST_CHECK(v.empty());
    BOOST_CHECK_EQUAL(v.size(), 0u);
    BOOST_CHECK_EQUAL(stats.constructCount, stats.destructCount);
}

BOOST_AUTO_TEST_CASE(RangeAssignTest)
{
}

BOOST_AUTO_TEST_CASE(FillAssignTest)
{
}

BOOST_AUTO_TEST_CASE(RangeInsertTest)
{
}

BOOST_AUTO_TEST_CASE(ElementInsertTest)
{
}

BOOST_AUTO_TEST_CASE(FillInsertTest)
{
}

BOOST_AUTO_TEST_CASE(ElementEraseTest)
{
}

BOOST_AUTO_TEST_CASE(RangeEraseTest)
{
}

BOOST_AUTO_TEST_CASE(ClearTest)
{
}

BOOST_AUTO_TEST_CASE(AccessTest)
{
}

BOOST_AUTO_TEST_CASE(SwapTest)
{
}

BOOST_AUTO_TEST_SUITE_END()

}}