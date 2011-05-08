#ifndef CRUNCH_BASE_OVERRIDE_HPP
#define CRUNCH_BASE_OVERRIDE_HPP

#include "crunch/base/platform.hpp"

#if defined(CRUNCH_COMPILER_MSVC)
#   define CRUNCH_OVERRIDE override
#else
#   define CRUNCH_OVERRIDE
#endif

#endif