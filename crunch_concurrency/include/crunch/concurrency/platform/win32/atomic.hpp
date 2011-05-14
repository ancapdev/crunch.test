#ifndef CRUNCH_CONCURRENCY_PLATFORM_WIN32_ATOMIC_HPP
#define CRUNCH_CONCURRENCY_PLATFORM_WIN32_ATOMIC_HPP

#include "crunch/base/platform.hpp"

#include "crunch/concurrency/platform/win32/atomic_type.hpp"

#if defined (CRUNCH_ARCH_X86)
#   include "crunch/concurrency/platform/win32/atomic_ops_x86.hpp"
#endif

#endif