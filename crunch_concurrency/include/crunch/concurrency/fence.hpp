#ifndef CRUNCH_CONCURRENCY_FENCE_HPP
#define CRUNCH_CONCURRENCY_FENCE_HPP

#include "crunch/base/platform.hpp"

#if defined (CRUNCH_PLATFORM_WIN32)
#   include <intrin.h>
#   include <Windows.h>
#   define CRUNCH_COMPILER_FENCE() _ReadWriteBarrier()
#   define CRUNCH_MEMORY_FENCE() MemoryBarrier()
#elif defined (CRUNCH_PLATFORM_LINUX)
#   define CRUNCH_COMPILER_FENCE() asm volatile ("" : : : "memory")
#   define CRUNCH_MEMORY_FENCE() __sync_synchronize()
#else
#   error "Unsupported platform"
#endif

#endif
