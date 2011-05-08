#ifndef CRUNCH_BASE_PLATFORM_HPP
#define CRUNCH_BASE_PLATFORM_HPP

#if defined (_WIN32)
#   define CRUNCH_PLATFORM_WINDOWS
#elif defined (LINUX)
#   define CRUNCH_PLATFORM_LINUX
#else
#   error "Unknown platform"
#endif

#if defined (_MSC_VER)
#   define CRUNCH_COMPILER_MSVC
#endif

#endif