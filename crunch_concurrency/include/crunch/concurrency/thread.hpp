// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_CONCURRENCY_THREAD_HPP
#define CRUNCH_CONCURRENCY_THREAD_HPP

#include "crunch/base/platform.hpp"

#if defined (CRUNCH_PLATFORM_WIN32)
#   include "crunch/base/platform/win32/wintypes.hpp"
#elif defined (CRUNCH_PLATFORM_LINUX)
#   include <sys/types.h>
#else
#   error "Unsupported platform"
#endif

namespace Crunch { namespace Concurrency {

#if defined (CRUNCH_PLATFORM_WIN32)
typedef HANDLE ThreadId;
typedef HANDLE ProcessId;
#else
typedef pid_t ThreadId;
typedef pid_t ProcessId;
#endif

ThreadId GetCurrentThreadId();
ProcessId GetCurrentProcessId();

}}

#endif
