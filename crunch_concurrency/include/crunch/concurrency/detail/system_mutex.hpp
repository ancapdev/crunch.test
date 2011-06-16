#ifndef CRUNCH_CONCURRENCY_DETAIL_SYSTEM_MUTEX_HPP
#define CRUNCH_CONCURRENCY_DETAIL_SYSTEM_MUTEX_HPP

#include "crunch/base/platform.hpp"
#include "crunch/concurrency/lock_guard.hpp"

#if defined (CRUNCH_PLATFORM_LINUX)
#   include <pthread.h>
#else
#   error "Unsupported platform"
#endif

namespace Crunch { namespace Concurrency { namespace Detail {

class SystemMutex
{
public:
    typedef LockGuard<SystemMutex> ScopedLock;

    SystemMutex();

    ~SystemMutex();

    bool TryLock();

    void Lock();

    void Unlock();

private:
#if defined (CRUNCH_PLATFORM_LINUX)
    pthread_mutex_t mMutex;
#endif
};


}}}

#endif
