// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_BASE_STACK_ALLOC_HPP
#define CRUNCH_BASE_STACK_ALLOC_HPP

#include "crunch/base/platform.hpp"

#if defined (CRUNCH_PLATFORM_WIN32)
#   include <malloc.h>
#   define CRUNCH_STACK_ALLOC(s) _alloca(s)
#elif defined (CRUNCH_PLATFORM_LINUX)
#   include <alloca.h>
#   define CRUNCH_STACK_ALLOC(s) alloca(s)
#else
#   error "Unsupported platform"
#endif

#define CRUNCH_STACK_ALLOC_T(t, c) (t*)CRUNCH_STACK_ALLOC(sizeof(t) * c)

#endif