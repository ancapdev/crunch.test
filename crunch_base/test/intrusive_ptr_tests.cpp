// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/base/intrusive_ptr.hpp"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

namespace Crunch {

namespace
{
    struct Pointee
    {
        Pointee() : count(0), maxcount(0), mincount(0) {}

        int count;
        int mincount;
        int maxcount;
    };

    void AddRef(Pointee* p)
    {
        p->count++;
        if (p->maxcount < p->count)
            p->maxcount = p->count;
    }

    void Release(Pointee* p)
    {
        p->count--;
        if (p->mincount > p->count)
            p->mincount = p->count;
    }

    typedef IntrusivePtr<Pointee> TestPtr;
}

BOOST_AUTO_TEST_SUITE(IntrusivePtrTests)

BOOST_AUTO_TEST_CASE(DefaultConstructTest)
{
    TestPtr p;
    BOOST_CHECK_EQUAL(p.Get(), (Pointee*)0);
    BOOST_CHECK_EQUAL(&*p, (Pointee*)0);
    BOOST_CHECK(!p);
}

BOOST_AUTO_TEST_CASE(ConstructAddRefTest)
{
    Pointee a;
    TestPtr p(&a);
    BOOST_CHECK_EQUAL(a.count, 1);
    BOOST_CHECK_EQUAL(a.maxcount, 1);
}

BOOST_AUTO_TEST_CASE(ConstructNoAddRefTest)
{
    Pointee a;
    TestPtr p(&a, false);
    BOOST_CHECK_EQUAL(a.count, 0);
    BOOST_CHECK_EQUAL(a.maxcount, 0);
}

BOOST_AUTO_TEST_CASE(DestructReleaseTest)
{
    Pointee a;
    {
        TestPtr p(&a);
        BOOST_CHECK_EQUAL(a.count, 1);
    }
    BOOST_CHECK_EQUAL(a.count, 0);
    BOOST_CHECK_EQUAL(a.mincount, 0);
}

BOOST_AUTO_TEST_CASE(CopyConstructTest)
{
    Pointee a;
    {
        TestPtr p0(&a);
        BOOST_CHECK_EQUAL(a.count, 1);

        TestPtr p1(p0);
        BOOST_CHECK_EQUAL(a.count, 2);
        BOOST_CHECK_EQUAL(a.maxcount, 2);
    }
    BOOST_CHECK_EQUAL(a.count, 0);
    BOOST_CHECK_EQUAL(a.mincount, 0);
}

BOOST_AUTO_TEST_CASE(MoveConstructTest)
{
    Pointee a;
    {
        TestPtr p0(&a);
        BOOST_CHECK_EQUAL(a.count, 1);
        a.mincount = 1;

        TestPtr p1(std::move(p0));
        BOOST_CHECK_EQUAL(a.count, 1);
        BOOST_CHECK_EQUAL(a.mincount, 1);
    }
    BOOST_CHECK_EQUAL(a.count, 0);
}

BOOST_AUTO_TEST_CASE(CopyAssignTest)
{
    Pointee a;
    {
        TestPtr p0(&a);
        BOOST_CHECK_EQUAL(a.count, 1);

        TestPtr p1;
        p1 = p0;
        BOOST_CHECK_EQUAL(a.count, 2);
    }
    BOOST_CHECK_EQUAL(a.count, 0);
}

BOOST_AUTO_TEST_CASE(MoveAssignTest)
{
    Pointee a;
    {
        TestPtr p0(&a);
        BOOST_CHECK_EQUAL(a.count, 1);
        a.mincount = 1;

        TestPtr p1;
        p1 = std::move(p0);
        BOOST_CHECK_EQUAL(a.count, 1);
        BOOST_CHECK_EQUAL(a.mincount, 1);
    }
    BOOST_CHECK_EQUAL(a.count, 0);
}

BOOST_AUTO_TEST_CASE(SelfAssignTest)
{
    Pointee a;
    {
        TestPtr p0(&a);
        BOOST_CHECK_EQUAL(a.count, 1);
        a.mincount = 1;

        p0 = p0;
        BOOST_CHECK_EQUAL(a.count, 1);
        BOOST_CHECK_EQUAL(a.mincount, 1);

        p0 = std::move(p0);
        BOOST_CHECK_EQUAL(a.count, 1);
        BOOST_CHECK_EQUAL(a.mincount, 1);
    }
    BOOST_CHECK_EQUAL(a.count, 0);
}

BOOST_AUTO_TEST_CASE(AccessorsTest)
{
    Pointee a;
    TestPtr p(&a);
    BOOST_CHECK_EQUAL(p.Get(), &a);
    BOOST_CHECK_EQUAL(&*p, &a);
    BOOST_CHECK((bool)p);
}

BOOST_AUTO_TEST_SUITE_END()

}
