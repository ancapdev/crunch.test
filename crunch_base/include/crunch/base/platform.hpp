// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_BASE_PLATFORM_HPP
#define CRUNCH_BASE_PLATFORM_HPP

#if defined (_WIN32)
#   define CRUNCH_PLATFORM_WIN32
#elif defined (__linux__)
#   define CRUNCH_PLATFORM_LINUX
#else
#   error "Unknown platform"
#endif

#if defined (_MSC_VER)
#   define CRUNCH_COMPILER_MSVC
#   if defined (_M_IX86)
#       define CRUNCH_ARCH_X86 32
#       define CRUNCH_ARCH_X86_32
#       define CRUNCH_PTR_SIZE 4
#   elif defined (_M_X64)
#       define CRUNCH_ARCH_X86 64
#       define CRUNCH_ARCH_X86_64
#       define CRUNCH_PTR_SIZE 8
#   elif defined (_M_IA64)
#       define CRUNCH_ARCH_IA64
#       define CRUNCH_PTR_SIZE 8
#   else
#       error "Unsupported architecture"
#   endif
#elif defined (__GNUC__)
#   define CRUNCH_COMPILER_GCC
#   define CRUNCH_COMPILER_VERSION_MAJOR __GNUC__
#   define CRUNCH_COMPILER_VERSION_MINOR __GNUC_MINOR__
#   define CRUNCH_COMPILER_VERSION_PATCH __GNUC_PATCHLEVEL__
#   define CRUNCH_COMPILER_MAKE_VERSION(major, minor, patch) (major * 1000000 + minor * 100 + patch)
#   if defined (__i386__)
#       define CRUNCH_ARCH_X86 32
#       define CRUNCH_ARCH_X86_32
#       define CRUNCH_PTR_SIZE 4
#   elif defined (__x86_64__)
#       define CRUNCH_ARCH_X86 64
#       define CRUNCH_ARCH_X86_64
#       define CRUNCH_PTR_SIZE 8
#   else
#       error "Unsupported architecture"
#   endif
#else
#   error "Unsupported compiler"
#endif

#define CRUNCH_COMPILER_VERSION \
    CRUNCH_COMPILER_MAKE_VERSION(      \
        CRUNCH_COMPILER_VERSION_MAJOR, \
        CRUNCH_COMPILER_VERSION_MINOR, \
        CRUNCH_COMPILER_VERSION_PATCH)

#endif
