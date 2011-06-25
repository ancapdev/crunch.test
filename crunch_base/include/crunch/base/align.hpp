// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_BASE_ALIGN_HPP
#define CRUNCH_BASE_ALIGN_HPP

#include "crunch/base/platform.hpp"

#if defined (CRUNCH_COMPILER_MSVC)
#   define CRUNCH_ALIGN_PREFIX(a) __declspec(align(a))
#   define CRUNCH_ALIGN_POSTFIX(a)
#elif defined (CRUNCH_COMPILER_GCC)
#   define CRUNCH_ALIGN_PREFIX(a)
#   define CRUNCH_ALIGN_POSTFIX(a) __attribute__((aligned(a)))
#else
#   error "Unsupported compiler"
#endif

#endif
