// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_CONCURRENCY_THREAD_HPP
#define CRUNCH_CONCURRENCY_THREAD_HPP

#include "crunch/base/platform.hpp"
#include "crunch/base/noncopyable.hpp"

#if defined (CRUNCH_PLATFORM_WIN32)
#   include "crunch/base/platform/win32/wintypes.hpp"
#elif defined (CRUNCH_PLATFORM_LINUX)
#   include <sys/types.h>
#   include <pthread.h>
#else
#   error "Unsupported platform"
#endif

#include <functional>

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


class Thread : NonCopyable
{
public:

#if defined (CRUNCH_PLATFORM_WIN32)
    typedef HANDLE PlatformHandleType;
#else
    typedef pthread_t PlatformHandleType;
#endif

    Thread()
        : mInitialized(false)
    {}

    ~Thread()
    {
        if (IsJoinable())
            Detach();
    }

    template<typename F>
    explicit Thread(F f)
    {
        Create(std::function<void ()>(f));
    }

    Thread(Thread&& rhs);
    Thread& operator = (Thread&& rhs);

    ThreadId GetId() const;

    bool IsJoinable() const
    {
        return mInitialized;
    }

    void Join();

    void Detach();

    // void Cancel();
    // bool IsCancellationRequested() const;

    PlatformHandleType GetPlatformHandle() const
    {
        return mHandle;
    }

private:
    void Create(std::function<void ()>&& f);

    bool mInitialized;
    PlatformHandleType mHandle;
};

}}

#endif
