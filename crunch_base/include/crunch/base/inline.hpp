// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_BASE_INLINE_HPP
#define CRUNCH_BASE_INLINE_HPP

#include "crunch/base/platform.hpp"

#if defined (CRUNCH_COMPILER_MSVC)
#   define CRUNCH_ALWAYS_INLINE __forceinline
#   define CRUNCH_NEVER_INLINE __declspec(noinline)
#elif defined (CRUNCH_COMPILER_GCC)
#   define CRUNCH_ALWAYS_INLINE __attribute__((always_inline))
#   define CRUNCH_NEVER_INLINE __attribute__((noinline))
#else
#   define CRUNCH_ALWAYS_INLINE inline
#   define CRUNCH_NEVER_INLINE
#endif

#endif
