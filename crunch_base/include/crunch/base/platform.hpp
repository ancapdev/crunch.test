#ifndef CRUNCH_BASE_PLATFORM_HPP
#define CRUNCH_BASE_PLATFORM_HPP

#if defined (_WIN32)
#   define CRUNCH_PLATFORM_WIN32
#elif defined (LINUX)
#   define CRUNCH_PLATFORM_LINUX
#else
#   error "Unknown platform"
#endif

#if defined (_MSC_VER)
#   define CRUNCH_COMPILER_MSVC
#   if defined (_M_IX86)
#       define CRUNCH_ARCH_X86 32
#       define CRUNCH_ARCH_X86_32
#   elif defined (_M_X64)
#       define CRUNCH_ARCH_X86 64
#       define CRUNCH_ARCH_X86_64
#   elif defined (_M_IA64)
#       define CRUNCH_ARCH_IA64
#   else
#       error "Unsupported architecture"
#   endif
#endif

#endif