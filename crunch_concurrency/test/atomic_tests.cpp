#include "crunch/concurrency/atomic.hpp"

#include <boost/mpl/list.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

namespace Crunch { namespace Concurrency {

typedef boost::mpl::list<
    boost::int16_t,
    boost::uint16_t,
    boost::int32_t,
    boost::uint32_t,
    boost::int64_t,
    boost::uint64_t> AtomicTypes;

typedef boost::mpl::list<
#if defined (CRUNCH_ARCH_X86_64)
    boost::int64_t,
    boost::uint64_t,
#endif
    boost::int32_t,
    boost::uint32_t> AtomicSwapTypes;

BOOST_AUTO_TEST_SUITE(AtomicTests)

BOOST_AUTO_TEST_CASE_TEMPLATE(LoadTest, T, AtomicTypes)
{
    __declspec(align(8)) T value = 123;
    BOOST_CHECK_EQUAL(value, AtomicLoad(value));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(StoreTest, T, AtomicTypes)
{
    __declspec(align(8)) T value = 0;
    AtomicStore(value, T(123));
    BOOST_CHECK_EQUAL(value, 123);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SwapTest, T, AtomicSwapTypes)
{
    __declspec(align(8)) T value = 0;
    T const old = AtomicSwap(value, T(123));
    BOOST_CHECK_EQUAL(old, 0);
    BOOST_CHECK_EQUAL(value, 123);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CompareAndSwapSucceedTest, T, AtomicTypes)
{
    __declspec(align(8)) T value = 0;
 
    T const old = AtomicCompareAndSwap(value, T(123), T(0));
    BOOST_CHECK_EQUAL(old, 0);
    BOOST_CHECK_EQUAL(value, 123);

    bool const swapped = AtomicCompareAndSwapTest(value, T(0), T(123));
    BOOST_CHECK(swapped);
    BOOST_CHECK_EQUAL(value, 0);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CompareAndSwapFailTest, T, AtomicTypes)
{
    __declspec(align(8)) T value = 0;
 
    T const old = AtomicCompareAndSwap(value, T(0), T(123));
    BOOST_CHECK_EQUAL(old, 0);
    BOOST_CHECK_EQUAL(value, 0);

    bool const swapped = AtomicCompareAndSwapTest(value, T(0), T(123));
    BOOST_CHECK(!swapped);
    BOOST_CHECK_EQUAL(value, 0);
}

BOOST_AUTO_TEST_SUITE_END()

}}
