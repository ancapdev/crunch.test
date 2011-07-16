// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_BASE_NORETURN_HPP
#define CRUNCH_BASE_NORETURN_HPP

#include "crunch/base/platform.hpp"

#if defined (CRUNCH_COMPILER_MSVC)
#   define CRUNCH_NORETURN __declspec(noreturn)
#elif defined (CRUNCH_COMPILER_GCC)
#   define CRUNCH_NORETURN __attribute__((noreturn))
#else
#   error "Unsupported compiler"
#endif

#endif
