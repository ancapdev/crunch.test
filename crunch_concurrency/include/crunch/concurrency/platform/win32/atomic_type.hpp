#ifndef CRUNCH_CONCURRENCY_PLATFORM_WIN32_ATOMIC_TYPE_HPP
#define CRUNCH_CONCURRENCY_PLATFORM_WIN32_ATOMIC_TYPE_HPP

#include "crunch/base/align.hpp"

#include <boost/static_assert.hpp>

#include <emmintrin.h>

namespace Crunch { namespace Concurrency { namespace Platform {

template<std::size_t S>
struct AtomicType;

template<>
struct AtomicType<1>
{
    typedef char Type;
    Type bits;
};

template<>
struct CRUNCH_ALIGN_PREFIX(2) AtomicType<2>
{
    typedef short Type;
    Type bits;
} CRUNCH_ALIGN_POSTFIX(2);

template<>
struct CRUNCH_ALIGN_PREFIX(4) AtomicType<4>
{
    typedef long Type;
    Type bits;
} CRUNCH_ALIGN_POSTFIX(4);

template<>
struct CRUNCH_ALIGN_PREFIX(8) AtomicType<8>
{
    typedef __int64 Type;
    Type bits;
} CRUNCH_ALIGN_POSTFIX(8);

template<>
struct CRUNCH_ALIGN_PREFIX(16) AtomicType<16>
{
    typedef __m128i Type;
    Type bits;
} CRUNCH_ALIGN_POSTFIX(16);

BOOST_STATIC_ASSERT(sizeof(AtomicType<1>) == 1);
BOOST_STATIC_ASSERT(sizeof(AtomicType<2>) == 2);
BOOST_STATIC_ASSERT(sizeof(AtomicType<4>) == 4);
BOOST_STATIC_ASSERT(sizeof(AtomicType<8>) == 8);
BOOST_STATIC_ASSERT(sizeof(AtomicType<16>) == 16);

}}}

#endif