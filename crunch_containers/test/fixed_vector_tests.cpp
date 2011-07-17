// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/containers/fixed_vector.hpp"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <stdexcept>

namespace Crunch { namespace Containers {


class Tracker
{
public:
    struct Statistics
    {
        Statistics() : constructCount(0), destructCount(0) {}

        int constructCount;
        int destructCount;
    };

    Tracker(Statistics& statistics) : mStatistics(statistics)
    {
        mStatistics.constructCount++;
    }

    Tracker(Tracker const& rhs) : mStatistics(rhs.mStatistics)
    {
        mStatistics.constructCount++;
    }

    Tracker& operator = (Tracker const& rhs)
    {
        mStatistics = rhs.mStatistics;
        return *this;
    }

    ~Tracker()
    {
        mStatistics.destructCount++;
    }

private:
    Statistics& mStatistics;
};

BOOST_AUTO_TEST_SUITE(FixedVectorTests)

BOOST_AUTO_TEST_CASE(ConstructEmptyTest)
{
    FixedVector<Tracker, 8> v;
    BOOST_CHECK(v.empty());
    BOOST_CHECK_EQUAL(v.begin(), v.end());
    BOOST_CHECK_EQUAL(&v[0], v.begin());
    BOOST_CHECK_EQUAL(v.size(), 0u);
    BOOST_CHECK_EQUAL(v.max_size(), 8u);
    BOOST_CHECK_EQUAL(v.capacity(), 8u);
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

BOOST_AUTO_TEST_SUITE_END()

}}