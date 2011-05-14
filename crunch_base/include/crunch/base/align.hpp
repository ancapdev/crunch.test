#ifndef CRUNCH_BASE_ALIGN_HPP
#define CRUNCH_BASE_ALIGN_HPP

#include "crunch/base/platform.hpp"

#if defined (CRUNCH_COMPILER_MSVC)
#   define CRUNCH_ALIGN_PREFIX(a) __declspec(align(n))
#   define CRUCNH_ALIGN_POSTFIX(a)
#else
#   error "Unsupported platform"
#endif

#endif