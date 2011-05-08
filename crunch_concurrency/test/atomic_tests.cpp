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

typedef AtomicSwapTypes AtomicAddTypes;

typedef boost::mpl::list<
#if defined (CRUNCH_ARCH_X86_64)
    boost::int64_t,
    boost::uint64_t,
#endif
    boost::int16_t,
    boost::uint16_t,
    boost::int32_t,
    boost::uint32_t> AtomicIncrementTypes;

typedef boost::mpl::list<
#if defined (CRUNCH_ARCH_X86_64)
    boost::int64_t,
    boost::uint64_t,
#endif
    boost::int8_t,
    boost::uint8_t,
    boost::int16_t,
    boost::uint16_t,
    boost::int32_t,
    boost::uint32_t> AtomicAndTypes;

typedef AtomicAndTypes AtomicOrTypes;
typedef AtomicAndTypes AtomicXorTypes;

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

BOOST_AUTO_TEST_CASE_TEMPLATE(AddTest, T, AtomicAddTypes)
{
    __declspec(align(8)) T value = 0;
   
    T old = AtomicAdd(value, T(123));
    BOOST_CHECK_EQUAL(old, 0);
    BOOST_CHECK_EQUAL(value, 123);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IncrementTest, T, AtomicIncrementTypes)
{
    __declspec(align(8)) T value = 0;

    T old = AtomicIncrement(value);
    BOOST_CHECK_EQUAL(old, 0);
    BOOST_CHECK_EQUAL(value, 1);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(DecrementTest, T, AtomicIncrementTypes)
{
    __declspec(align(8)) T value = 1;

    T old = AtomicDecrement(value);
    BOOST_CHECK_EQUAL(old, 1);
    BOOST_CHECK_EQUAL(value, 0);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(AndTest, T, AtomicAndTypes)
{
    __declspec(align(8)) T value = 0x48;

    T old = AtomicAnd(value, T(0x42));
    BOOST_CHECK_EQUAL(old, 0x48);
    BOOST_CHECK_EQUAL(value, 0x40);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(OrTest, T, AtomicOrTypes)
{
    __declspec(align(8)) T value = 0x40;

    T old = AtomicOr(value, T(0x08));
    BOOST_CHECK_EQUAL(old, 0x40);
    BOOST_CHECK_EQUAL(value, 0x48);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(XorTest, T, AtomicXorTypes)
{
    __declspec(align(8)) T value = 0x48;

    T old = AtomicXor(value, T(0x28));
    BOOST_CHECK_EQUAL(old, 0x48);
    BOOST_CHECK_EQUAL(value, 0x60);
}

BOOST_AUTO_TEST_SUITE_END()

}}
