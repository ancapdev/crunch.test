#ifndef CRUNCH_BASE_STDINT_HPP
#define CRUNCH_BASE_STDINT_HPP

#include "crunch/base/platform.hpp"

#if defined (CRUNCH_PLATFORM_LINUX)
#   include <cstdint>
#endif

namespace Crunch {

#if defined (CRUNCH_PLATFORM_LINUX)

typedef std::int8_t int8;
typedef std::uint8_t uint8;
typedef std::int16_t int16;
typedef std::uint16_t uint16;
typedef std::int32_t int32;
typedef std::uint32_t uint32;
typedef std::int64_t int64;
typedef std::uint64_t uint64;

#elif defined (CRUNCH_PLATFORM_WIN32)

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef long int32;
typedef unsigned long uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;

#else
#   error "Unsupported platform"
#endif

}

#endif
