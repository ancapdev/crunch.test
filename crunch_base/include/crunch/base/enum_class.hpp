// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_BASE_ENUM_CLASS_HPP
#define CRUNCH_BASE_ENUM_CLASS_HPP

#include "crunch/base/platform.hpp"

// Work around MSVC partial support for type safe enums
#if defined (CRUNCH_COMPILER_MSVC)
#   define CRUNCH_ENUM_CLASS enum
#else
#   define CRUNCH_ENUM_CLASS enum class
#endif

#endif
