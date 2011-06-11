#ifndef CRUNCH_BASE_DEBUG_HPP
#define CRUNCH_BASE_DEBUG_HPP

#include "crunch/base/platform.hpp"

#if defined (CRUNCH_COMPILER_MSVC)
#   define CRUNCH_HALT() __debugbreak()
#else
#   define CRUNCH_HALT() *(int*)0 = 0
#endif

#if defined (NDEBUG)
#   define CRUNCH_RELEASE
#else
#   define CRUNCH_DEBUG
#endif

#endif