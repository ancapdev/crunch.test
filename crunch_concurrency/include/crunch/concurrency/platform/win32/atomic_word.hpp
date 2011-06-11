#ifndef CRUNCH_CONCURRENCY_PLATFORM_WIN32_ATOMIC_TYPE_HPP
#define CRUNCH_CONCURRENCY_PLATFORM_WIN32_ATOMIC_TYPE_HPP

#include <boost/static_assert.hpp>

#include <emmintrin.h>

namespace Crunch { namespace Concurrency { namespace Platform {

template<std::size_t S> struct AtomicWordHelper;
template<> struct AtomicWordHelper<1> { typedef char Type; };
template<> struct AtomicWordHelper<2> { typedef short Type; };
template<> struct AtomicWordHelper<4> { typedef long Type; };
template<> struct AtomicWordHelper<8> { typedef __int64 Type; };
template<> struct AtomicWordHelper<16> { typedef __m128i Type; };

BOOST_STATIC_ASSERT(sizeof(AtomicWordHelper<1>::Type) == 1);
BOOST_STATIC_ASSERT(sizeof(AtomicWordHelper<2>::Type) == 2);
BOOST_STATIC_ASSERT(sizeof(AtomicWordHelper<4>::Type) == 4);
BOOST_STATIC_ASSERT(sizeof(AtomicWordHelper<8>::Type) == 8);
BOOST_STATIC_ASSERT(sizeof(AtomicWordHelper<16>::Type) == 16);

template<typename T> struct AtomicWord : AtomicWordHelper<sizeof(T)> {};

}}}

#endif