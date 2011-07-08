// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_CONCURRENCY_DETAIL_SYSTEM_SEMAPHORE_HPP
#define CRUNCH_CONCURRENCY_DETAIL_SYSTEM_SEMAPHORE_HPP

#include "crunch/base/platform.hpp"
#include "crunch/base/stdint.hpp"

#if defined (CRUNCH_PLATFORM_WIN32)
#   include "crunch/base/platform/win32/wintypes.hpp"
#   include "crunch/concurrency/atomic.hpp"
#elif defined (CRUNCH_PLATFORM_LINUX)
#   include <semaphore.h>
#else
#   error "Unsupported platform"
#endif

namespace Crunch { namespace Concurrency { namespace Detail {

class SystemSemaphore
{
public:
    SystemSemaphore(uint32 initialCount);
    ~SystemSemaphore();

    void Post();
    void Wait();

private:
#if defined (CRUNCH_PLATFORM_WIN32)
    Atomic<int32> mCount;
    HANDLE mSemaphore;
#else
    sem_t mSemaphore;
#endif
};

}}}

#endif
