#include "crunch/concurrency/atomic.hpp"

// TODO: remove
#include "crunch/concurrency/lock_free_slist.hpp"

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
    T v = 123;
    Atomic<T> a;
    std::memcpy(&a, &v, sizeof(T));
    BOOST_CHECK_EQUAL(a.Load(), 123);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(StoreTest, T, AtomicTypes)
{
    Atomic<T> a;
    a.Store(123);
    T v;
    std::memcpy(&v, &a, sizeof(T));
    BOOST_CHECK_EQUAL(v, 123);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SwapTest, T, AtomicSwapTypes)
{
    Atomic<T> value = 102;
    T const old = value.Swap(T(123));
    BOOST_CHECK_EQUAL(old, 102);
    BOOST_CHECK_EQUAL(value, 123);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CompareAndSwapSucceedTest, T, AtomicTypes)
{
    Atomic<T> value = 0;

    T cmp = 0;
    bool const swapped = value.CompareAndSwap(123, cmp);
    BOOST_CHECK(swapped);
    BOOST_CHECK_EQUAL(cmp, 0);
    BOOST_CHECK_EQUAL(value, 123);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CompareAndSwapFailTest, T, AtomicTypes)
{
    Atomic<T> value = 0;
 
    T cmp = 123;
    bool const swapped = value.CompareAndSwap(0, cmp);
    BOOST_CHECK(!swapped);
    BOOST_CHECK_EQUAL(cmp, 0);
    BOOST_CHECK_EQUAL(value, 0);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(AddTest, T, AtomicAddTypes)
{
    Atomic<T> value = 0;
   
    T old = value.Add(T(123));
    BOOST_CHECK_EQUAL(old, 0);
    BOOST_CHECK_EQUAL(value, 123);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IncrementTest, T, AtomicIncrementTypes)
{
    Atomic<T> value = 0;

    T old = value.Increment();
    BOOST_CHECK_EQUAL(old, 0);
    BOOST_CHECK_EQUAL(value, 1);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(DecrementTest, T, AtomicIncrementTypes)
{
    Atomic<T> value = 1;

    T old = value.Decrement();
    BOOST_CHECK_EQUAL(old, 1);
    BOOST_CHECK_EQUAL(value, 0);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(AndTest, T, AtomicAndTypes)
{
    Atomic<T> value = 0x48;

    T old = value.And(T(0x42));
    BOOST_CHECK_EQUAL(old, 0x48);
    BOOST_CHECK_EQUAL(value, 0x40);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(OrTest, T, AtomicOrTypes)
{
    Atomic<T> value = 0x40;

    T old = value.Or(T(0x08));
    BOOST_CHECK_EQUAL(old, 0x40);
    BOOST_CHECK_EQUAL(value, 0x48);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(XorTest, T, AtomicXorTypes)
{
    Atomic<T> value = 0x48;

    T old = value.Xor(T(0x28));
    BOOST_CHECK_EQUAL(old, 0x48);
    BOOST_CHECK_EQUAL(value, 0x60);
}

BOOST_AUTO_TEST_SUITE_END()

}}
