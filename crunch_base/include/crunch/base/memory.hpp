#ifndef CRUNCH_BASE_MEMORY_HPP
#define CRUNCH_BASE_MEMORY_HPP

#include "crunch/base/platform.hpp"

#include <cstddef>
#include <malloc.h>

namespace Crunch {

#if defined (CRUNCH_PLATFORM_WIN32)

__declspec(noalias) __declspec(restrict) inline void* MallocAligned(std::size_t bytes, std::size_t alignment)
{
    return ::_aligned_malloc(bytes, alignment);
}

__declspec(noalias) inline void FreeAligned(void* allocation)
{
    ::_aligned_free(allocation);
}

#elif defined (CRUNCH_PLATFORM_LINUX)

inline void* MallocAligned(std::size_t bytes, std::size_t alignment)
{
    return ::memalign(alignment, bytes);
}

inline void FreeAligned(void* allocation)
{
    ::free(allocation);
}

#else
#   error "Unsupported platform"
#endif



inline void* AlignPointerUp(void* pointer, std::size_t alignment)
{
    std::size_t const a = alignment - 1;
    return (void*)(((std::size_t)pointer + a) & ~a);
}

inline void* AlignPointerDown(void* pointer, std::size_t alignment)
{
    return (void*)((std::size_t)pointer & ~(alignment - 1));
}

inline bool IsPointerAligned(void* pointer, std::size_t alignment)
{
    return ((std::size_t)pointer & (alignment - 1)) == 0;
}

inline void* OffsetPointer(void* pointer, std::ptrdiff_t delta)
{
    return (void*)((std::size_t)pointer + delta);
}

}

#endif
