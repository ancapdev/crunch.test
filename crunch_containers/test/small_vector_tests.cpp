// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/containers/small_vector.hpp"
#include "./tracker.hpp"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <stdexcept>

namespace Crunch { namespace Containers {

BOOST_AUTO_TEST_SUITE(SmallVectorTests)

BOOST_AUTO_TEST_CASE(DefaultConstructTest)
{
    SmallVector<Tracker, 8> const v;
    BOOST_CHECK(v.empty());
    BOOST_CHECK_EQUAL(v.begin(), v.end());
    BOOST_CHECK_EQUAL(&v[0], v.begin());
    BOOST_CHECK_EQUAL(v.size(), 0u);
    BOOST_CHECK_EQUAL(v.capacity(), 8u);
}

BOOST_AUTO_TEST_CASE(FillConstructTest)
{
    Tracker::Statistics stats;
    {
        SmallVector<Tracker, 8> const v(3, Tracker(stats, 123));
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
        SmallVector<Tracker, 8> const v(trackers, trackers + 3);
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
        SmallVector<Tracker, 8> const v(3, Tracker(stats, 123));
        SmallVector<Tracker, 8> const v2(v);
        BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.end(), v2.begin(), v2.end());
    }
    BOOST_CHECK_EQUAL(stats.constructCount, stats.destructCount);
}

BOOST_AUTO_TEST_CASE(MoveConstructTest)
{
    Tracker::Statistics stats;
    {
        SmallVector<Tracker, 8> v(3, Tracker(stats, 123));
        SmallVector<Tracker, 8> const v2(std::move(v));
        BOOST_CHECK(v.empty());
        BOOST_CHECK_EQUAL(v2.size(), 3u);
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
        SmallVector<Tracker, 8> const v(3, Tracker(stats, 123));
        SmallVector<Tracker, 8> v2;
        v2 = v;
        BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.end(), v2.begin(), v2.end());
    }
    BOOST_CHECK_EQUAL(stats.constructCount, stats.destructCount);

}

BOOST_AUTO_TEST_CASE(MoveAssignTest)
{
    Tracker::Statistics stats;
    {
        SmallVector<Tracker, 8> v(3, Tracker(stats, 123));
        SmallVector<Tracker, 8> v2;
        v2 = std::move(v);
        BOOST_CHECK(v.empty());
        BOOST_CHECK_EQUAL(v2.size(), 3u);
        BOOST_CHECK_EQUAL(v2[0].GetTag(), 123);
        BOOST_CHECK_EQUAL(v2[1].GetTag(), 123);
        BOOST_CHECK_EQUAL(v2[2].GetTag(), 123);
    }
    BOOST_CHECK_EQUAL(stats.constructCount, stats.destructCount);
}

BOOST_AUTO_TEST_CASE(RangeAssignTest)
{
    Tracker::Statistics stats;
    {
        Tracker const trackers[3] =
        {
            Tracker(stats, 1),
            Tracker(stats, 2),
            Tracker(stats, 3)
        };
        SmallVector<Tracker, 8> v;
        v.assign(trackers, trackers + 3);
        BOOST_CHECK_EQUAL(v.size(), 3u);
        BOOST_CHECK_EQUAL(v[0].GetTag(), 1);
        BOOST_CHECK_EQUAL(v[1].GetTag(), 2);
        BOOST_CHECK_EQUAL(v[2].GetTag(), 3);
    }
    BOOST_CHECK_EQUAL(stats.constructCount, stats.destructCount);
}

BOOST_AUTO_TEST_CASE(FillAssignTest)
{
    Tracker::Statistics stats;
    {
        SmallVector<Tracker, 8> v;
        v.assign(3, Tracker(stats, 123));
        BOOST_CHECK_EQUAL(v.size(), 3u);
        BOOST_CHECK_EQUAL(v[0].GetTag(), 123);
        BOOST_CHECK_EQUAL(v[1].GetTag(), 123);
        BOOST_CHECK_EQUAL(v[2].GetTag(), 123);
    }
    BOOST_CHECK_EQUAL(stats.constructCount, stats.destructCount);
}

BOOST_AUTO_TEST_CASE(ElementInsertTest)
{
    Tracker::Statistics stats;
    {
        SmallVector<Tracker, 4> v;
        
        v.insert(v.end(), Tracker(stats, 1));
        BOOST_CHECK_EQUAL(v.size(), 1u);
        BOOST_CHECK_EQUAL(v[0], Tracker(stats, 1));
        
        v.insert(v.end(), Tracker(stats, 2));
        BOOST_CHECK_EQUAL(v.size(), 2u);
        BOOST_CHECK_EQUAL(v[0], Tracker(stats, 1));
        BOOST_CHECK_EQUAL(v[1], Tracker(stats, 2));

        v.insert(v.begin(), Tracker(stats, 3));
        BOOST_CHECK_EQUAL(v.size(), 3u);
        BOOST_CHECK_EQUAL(v[0], Tracker(stats, 3));
        BOOST_CHECK_EQUAL(v[1], Tracker(stats, 1));
        BOOST_CHECK_EQUAL(v[2], Tracker(stats, 2));

        v.insert(v.begin() + 1, Tracker(stats, 4));
        BOOST_CHECK_EQUAL(v.size(), 4u);
        BOOST_CHECK_EQUAL(v[0], Tracker(stats, 3));
        BOOST_CHECK_EQUAL(v[1], Tracker(stats, 4));
        BOOST_CHECK_EQUAL(v[2], Tracker(stats, 1));
        BOOST_CHECK_EQUAL(v[3], Tracker(stats, 2));

        // Insert above embedded capacity
        v.insert(v.end(), Tracker(stats, 5));
        BOOST_CHECK_EQUAL(v.size(), 5u);
        BOOST_CHECK_EQUAL(v[0], Tracker(stats, 3));
        BOOST_CHECK_EQUAL(v[1], Tracker(stats, 4));
        BOOST_CHECK_EQUAL(v[2], Tracker(stats, 1));
        BOOST_CHECK_EQUAL(v[3], Tracker(stats, 2));
        BOOST_CHECK_EQUAL(v[4], Tracker(stats, 5));
    }
    BOOST_CHECK_EQUAL(stats.constructCount, stats.destructCount);
}

BOOST_AUTO_TEST_CASE(FillInsertTest)
{
    Tracker::Statistics stats;
    {
        SmallVector<Tracker, 12> v;

        // in empty
        v.insert(v.begin(), 3, Tracker(stats, 1));
        BOOST_CHECK_EQUAL(v.size(), 3u);
        BOOST_CHECK_EQUAL(v[0], Tracker(stats, 1));
        BOOST_CHECK_EQUAL(v[1], Tracker(stats, 1));
        BOOST_CHECK_EQUAL(v[2], Tracker(stats, 1));

        // at end
        v.insert(v.end(), 3, Tracker(stats, 2));
        BOOST_CHECK_EQUAL(v.size(), 6u);
        BOOST_CHECK_EQUAL(v[0], Tracker(stats, 1));
        BOOST_CHECK_EQUAL(v[1], Tracker(stats, 1));
        BOOST_CHECK_EQUAL(v[2], Tracker(stats, 1));
        BOOST_CHECK_EQUAL(v[3], Tracker(stats, 2));
        BOOST_CHECK_EQUAL(v[4], Tracker(stats, 2));
        BOOST_CHECK_EQUAL(v[5], Tracker(stats, 2));

        // at begin
        v.insert(v.begin(), 3, Tracker(stats, 3));
        BOOST_CHECK_EQUAL(v.size(), 9u);
        BOOST_CHECK_EQUAL(v[0], Tracker(stats, 3));
        BOOST_CHECK_EQUAL(v[1], Tracker(stats, 3));
        BOOST_CHECK_EQUAL(v[2], Tracker(stats, 3));
        BOOST_CHECK_EQUAL(v[3], Tracker(stats, 1));
        BOOST_CHECK_EQUAL(v[4], Tracker(stats, 1));
        BOOST_CHECK_EQUAL(v[5], Tracker(stats, 1));
        BOOST_CHECK_EQUAL(v[6], Tracker(stats, 2));
        BOOST_CHECK_EQUAL(v[7], Tracker(stats, 2));
        BOOST_CHECK_EQUAL(v[8], Tracker(stats, 2));

        // in middle
        v.insert(v.begin() + 1, 3, Tracker(stats, 4));
        BOOST_CHECK_EQUAL(v.size(), 12u);
        BOOST_CHECK_EQUAL(v[0], Tracker(stats, 3));
        BOOST_CHECK_EQUAL(v[1], Tracker(stats, 4));
        BOOST_CHECK_EQUAL(v[2], Tracker(stats, 4));
        BOOST_CHECK_EQUAL(v[3], Tracker(stats, 4));
        BOOST_CHECK_EQUAL(v[4], Tracker(stats, 3));
        BOOST_CHECK_EQUAL(v[5], Tracker(stats, 3));
        BOOST_CHECK_EQUAL(v[6], Tracker(stats, 1));
        BOOST_CHECK_EQUAL(v[7], Tracker(stats, 1));
        BOOST_CHECK_EQUAL(v[8], Tracker(stats, 1));
        BOOST_CHECK_EQUAL(v[9], Tracker(stats, 2));
        BOOST_CHECK_EQUAL(v[10], Tracker(stats, 2));
        BOOST_CHECK_EQUAL(v[11], Tracker(stats, 2));

        // above embedded capacity
        v.insert(v.end(), 3, Tracker(stats, 5));
        BOOST_REQUIRE_EQUAL(v.size(), 15u);
        BOOST_CHECK_EQUAL(v[0], Tracker(stats, 3));
        BOOST_CHECK_EQUAL(v[1], Tracker(stats, 4));
        BOOST_CHECK_EQUAL(v[2], Tracker(stats, 4));
        BOOST_CHECK_EQUAL(v[3], Tracker(stats, 4));
        BOOST_CHECK_EQUAL(v[4], Tracker(stats, 3));
        BOOST_CHECK_EQUAL(v[5], Tracker(stats, 3));
        BOOST_CHECK_EQUAL(v[6], Tracker(stats, 1));
        BOOST_CHECK_EQUAL(v[7], Tracker(stats, 1));
        BOOST_CHECK_EQUAL(v[8], Tracker(stats, 1));
        BOOST_CHECK_EQUAL(v[9], Tracker(stats, 2));
        BOOST_CHECK_EQUAL(v[10], Tracker(stats, 2));
        BOOST_CHECK_EQUAL(v[11], Tracker(stats, 2));
        BOOST_CHECK_EQUAL(v[12], Tracker(stats, 5));
        BOOST_CHECK_EQUAL(v[13], Tracker(stats, 5));
        BOOST_CHECK_EQUAL(v[14], Tracker(stats, 5));
    }
    BOOST_CHECK_EQUAL(stats.constructCount, stats.destructCount);
}

BOOST_AUTO_TEST_CASE(RangeInsertTest)
{
    Tracker::Statistics stats;
    {
        Tracker const trackers[3] =
        {
            Tracker(stats, 1),
            Tracker(stats, 2),
            Tracker(stats, 3)
        };

        SmallVector<Tracker, 12> v;

        // in empty
        v.insert(v.begin(), trackers, trackers + 3);
        BOOST_CHECK_EQUAL(v.size(), 3u);
        BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.end(), trackers, trackers + 3);

        // at end
        v.insert(v.end(), trackers, trackers + 3);
        BOOST_CHECK_EQUAL(v.size(), 6u);
        BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.begin() + 3, trackers, trackers + 3);
        BOOST_CHECK_EQUAL_COLLECTIONS(v.begin() + 3, v.end(), trackers, trackers + 3);

        // at begin
        v.insert(v.begin(), trackers, trackers + 3);
        BOOST_CHECK_EQUAL(v.size(), 9u);
        BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.begin() + 3, trackers, trackers + 3);
        BOOST_CHECK_EQUAL_COLLECTIONS(v.begin() + 3, v.begin() + 6, trackers, trackers + 3);
        BOOST_CHECK_EQUAL_COLLECTIONS(v.begin() + 6, v.end(), trackers, trackers + 3);

        // in middle
        v.insert(v.begin() + 1, trackers, trackers + 3);
        BOOST_CHECK_EQUAL(v.size(), 12u);
        BOOST_CHECK_EQUAL(v[0], trackers[0]);
        BOOST_CHECK_EQUAL_COLLECTIONS(v.begin() + 1, v.begin() + 4, trackers, trackers + 3);
        BOOST_CHECK_EQUAL_COLLECTIONS(v.begin() + 4, v.begin() + 6, trackers + 1, trackers + 3);
        BOOST_CHECK_EQUAL_COLLECTIONS(v.begin() + 6, v.begin() + 9, trackers, trackers + 3);
        BOOST_CHECK_EQUAL_COLLECTIONS(v.begin() + 9, v.end(), trackers, trackers + 3);

        // above embedded capacity
        v.insert(v.end(), trackers, trackers + 3);
        BOOST_REQUIRE_EQUAL(v.size(), 15u);
        BOOST_CHECK_EQUAL(v[0], trackers[0]);
        BOOST_CHECK_EQUAL_COLLECTIONS(v.begin() + 1, v.begin() + 4, trackers, trackers + 3);
        BOOST_CHECK_EQUAL_COLLECTIONS(v.begin() + 4, v.begin() + 6, trackers + 1, trackers + 3);
        BOOST_CHECK_EQUAL_COLLECTIONS(v.begin() + 6, v.begin() + 9, trackers, trackers + 3);
        BOOST_CHECK_EQUAL_COLLECTIONS(v.begin() + 9, v.begin() + 12, trackers, trackers + 3);
        BOOST_CHECK_EQUAL_COLLECTIONS(v.begin() + 12, v.end(), trackers, trackers + 3);
    }
    BOOST_CHECK_EQUAL(stats.constructCount, stats.destructCount);
}

BOOST_AUTO_TEST_CASE(ElementEraseTest)
{
    Tracker::Statistics stats;
    SmallVector<Tracker, 4> v;
    for (int i = 0; i < 4; ++i)
        v.push_back(Tracker(stats, i));

    // at begin
    BOOST_CHECK_EQUAL(v.erase(v.begin()), v.begin());
    BOOST_CHECK_EQUAL(v.size(), 3u);
    BOOST_CHECK_EQUAL(v[0], Tracker(stats, 1));
    BOOST_CHECK_EQUAL(v[1], Tracker(stats, 2));
    BOOST_CHECK_EQUAL(v[2], Tracker(stats, 3));

    // in middle
    BOOST_CHECK_EQUAL(v.erase(v.begin() + 1), v.begin() + 1);
    BOOST_CHECK_EQUAL(v.size(), 2u);
    BOOST_CHECK_EQUAL(v[0], Tracker(stats, 1));
    BOOST_CHECK_EQUAL(v[1], Tracker(stats, 3));
    
    // at end
    BOOST_CHECK_EQUAL(v.erase(v.begin() + 1), v.begin() + 1);
    BOOST_CHECK_EQUAL(v.size(), 1u);
    BOOST_CHECK_EQUAL(v[0], Tracker(stats, 1));

    // last
    BOOST_CHECK_EQUAL(v.erase(v.begin()), v.begin());
    BOOST_CHECK(v.empty());

    BOOST_CHECK_EQUAL(stats.constructCount, stats.destructCount);
}

BOOST_AUTO_TEST_CASE(RangeEraseTest)
{
    Tracker::Statistics stats;
    SmallVector<Tracker, 8> v;
    for (int i = 0; i < 8; ++i)
        v.push_back(Tracker(stats, i));

    // at begin
    BOOST_CHECK_EQUAL(v.erase(v.begin(), v.begin() + 2), v.begin());
    BOOST_CHECK_EQUAL(v.size(), 6u);
    BOOST_CHECK_EQUAL(v[0], Tracker(stats, 2));
    BOOST_CHECK_EQUAL(v[1], Tracker(stats, 3));
    BOOST_CHECK_EQUAL(v[2], Tracker(stats, 4));
    BOOST_CHECK_EQUAL(v[3], Tracker(stats, 5));
    BOOST_CHECK_EQUAL(v[4], Tracker(stats, 6));
    BOOST_CHECK_EQUAL(v[5], Tracker(stats, 7));

    // in middle
    BOOST_CHECK_EQUAL(v.erase(v.begin() + 1, v.begin() + 3), v.begin() + 1);
    BOOST_CHECK_EQUAL(v.size(), 4u);
    BOOST_CHECK_EQUAL(v[0], Tracker(stats, 2));
    BOOST_CHECK_EQUAL(v[1], Tracker(stats, 5));
    BOOST_CHECK_EQUAL(v[2], Tracker(stats, 6));
    BOOST_CHECK_EQUAL(v[3], Tracker(stats, 7));

    // at end
    BOOST_CHECK_EQUAL(v.erase(v.begin() + 2, v.end()), v.begin() + 2);
    BOOST_CHECK_EQUAL(v.size(), 2u);
    BOOST_CHECK_EQUAL(v[0], Tracker(stats, 2));
    BOOST_CHECK_EQUAL(v[1], Tracker(stats, 5));

    // last
    BOOST_CHECK_EQUAL(v.erase(v.begin(), v.end()), v.begin());
    BOOST_CHECK(v.empty());

    BOOST_CHECK_EQUAL(stats.constructCount, stats.destructCount);
}

BOOST_AUTO_TEST_CASE(ClearTest)
{
    Tracker::Statistics stats;
    SmallVector<Tracker, 8> v(4, Tracker(stats, 1));
    v.clear();
    BOOST_CHECK(v.empty());
    BOOST_CHECK_EQUAL(stats.constructCount, stats.destructCount);
}

BOOST_AUTO_TEST_CASE(PushPopTest)
{
    Tracker::Statistics stats;
    SmallVector<Tracker, 1> v;
    
    v.push_back(Tracker(stats, 1));
    BOOST_CHECK_EQUAL(stats.constructCount, stats.destructCount + 1);
    BOOST_CHECK(!v.empty());
    BOOST_CHECK_EQUAL(v.size(), 1u);
    BOOST_CHECK_EQUAL(v.end(), v.begin() + 1);
    BOOST_CHECK_EQUAL(v[0], Tracker(stats, 1));

    v.push_back(Tracker(stats, 2));
    BOOST_REQUIRE_EQUAL(v.size(), 2u);
    BOOST_CHECK_EQUAL(v.end(), v.begin() + 2);
    BOOST_CHECK_EQUAL(v[0], Tracker(stats, 1));
    BOOST_CHECK_EQUAL(v[1], Tracker(stats, 2));
    
    v.pop_back();
    v.pop_back();
    BOOST_CHECK(v.empty());
    BOOST_CHECK_EQUAL(v.size(), 0u);
    BOOST_CHECK_EQUAL(stats.constructCount, stats.destructCount);
}

BOOST_AUTO_TEST_CASE(AccessorTest)
{
    SmallVector<int, 4> v;
    for (int i = 0; i < 4; ++i)
        v.push_back(i);

    BOOST_CHECK_EQUAL(v.front(), 0);
    BOOST_CHECK_EQUAL(v.back(), 3);
    BOOST_CHECK_EQUAL(v[0], 0);
    BOOST_CHECK_EQUAL(v[1], 1);
    BOOST_CHECK_EQUAL(v[2], 2);
    BOOST_CHECK_EQUAL(v[3], 3);
    BOOST_CHECK_EQUAL(&v[0], v.begin());
    BOOST_CHECK_EQUAL(v.at(0), 0);
    BOOST_CHECK_EQUAL(v.at(1), 1);
    BOOST_CHECK_EQUAL(v.at(2), 2);
    BOOST_CHECK_EQUAL(v.at(3), 3);
    BOOST_CHECK_THROW(v.at(4), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(SwapTest)
{
    Tracker::Statistics stats;
    {
        SmallVector<Tracker, 4> v1;
        SmallVector<Tracker, 4> v2;

        for (int i = 0; i < 4; ++i)
            v1.push_back(Tracker(stats, i));

        v1.swap(v2);

        BOOST_CHECK(v1.empty());
        BOOST_CHECK_EQUAL(v2.size(), 4u);
        BOOST_CHECK_EQUAL(v2[0], Tracker(stats, 0));
        BOOST_CHECK_EQUAL(v2[1], Tracker(stats, 1));
        BOOST_CHECK_EQUAL(v2[2], Tracker(stats, 2));
        BOOST_CHECK_EQUAL(v2[3], Tracker(stats, 3));
    }
    BOOST_CHECK_EQUAL(stats.constructCount, stats.destructCount);
}

BOOST_AUTO_TEST_SUITE_END()

}}