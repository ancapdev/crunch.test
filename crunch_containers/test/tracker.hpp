// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_CONTAINERS_TEST_TRACKER_HPP
#define CRUNCH_CONTAINERS_TEST_TRACKER_HPP

#include <iosfwd>

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

    Tracker(Statistics& statistics, int tag = 0) 
        : mStatistics(statistics)
        , mTag(tag)
    {
        mStatistics.constructCount++;
    }

    Tracker(Tracker const& rhs)
        : mStatistics(rhs.mStatistics)
        , mTag(rhs.mTag)
    {
        mStatistics.constructCount++;
    }

    Tracker& operator = (Tracker const& rhs)
    {
        mStatistics = rhs.mStatistics;
        mTag = rhs.mTag;
        return *this;
    }

    ~Tracker()
    {
        mStatistics.destructCount++;
    }

    Statistics& GetStatistics() const { return mStatistics; }

    int GetTag() const { return mTag; }

private:
    Statistics& mStatistics;
    int mTag;
};

std::ostream& operator << (std::ostream& os, Tracker const& tracker);

inline bool operator == (Tracker const& lhs, Tracker const& rhs)
{
    return
        &lhs.GetStatistics() == &rhs.GetStatistics() &&
        lhs.GetTag() == rhs.GetTag();
}

inline bool operator != (Tracker const& lhs, Tracker const& rhs)
{
    return !(lhs == rhs);
}

}}

#endif