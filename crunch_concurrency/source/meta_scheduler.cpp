// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/concurrency/meta_scheduler.hpp"

#include "crunch/base/assert.hpp"
#include "crunch/base/inline.hpp"
#include "crunch/base/noncopyable.hpp"
#include "crunch/base/override.hpp"
#include "crunch/base/stack_alloc.hpp"
#include "crunch/concurrency/event.hpp"
#include "crunch/concurrency/yield.hpp"
#include "crunch/concurrency/detail/system_semaphore.hpp"

#include <algorithm>

namespace Crunch { namespace Concurrency {

class MetaScheduler::ContextImpl : public MetaScheduler::Context, NonCopyable
{
public:
    ContextImpl(MetaScheduler* owner)
        : mOwner(owner)
        , mWaitSemaphore(0)
        , mRefCount(1)
    {}

    MetaScheduler* GetOwner()
    {
        return mOwner;
    }

    void AddRef()
    {
        mRefCount++;
    }

    void Run(IWaitable& until)
    {
        // while not done
        //     Acquire idle meta thread
        //     Affinitize to meta thread processor affinity
        //     Set meta thread in TLS context
        //     Add schedulers for meta thread
        // 1:  Run schedulers until idle
        //         If blocked (from within scheduler -- can't switch meta threads at this point)
        //             If supported, re-enter scheduler until idle or not blocked or done
        //             Release meta thread (put in high demand idle list to increase chance of re-acquiring)
        //             Yield until not blocked or done
        //             Wait for same meta thread to become available (actually.. must be able to migrate meta thread at this point or might starve)
        //         If only 1 scheduler active
        //             Run scheduler until done or meta thread required by other blocked threads
        //     Release meta thread
        //     Yield until not-idle or done
        //     Attempt to re-acquire same meta thread
        //         If successful
        //             Goto 1:
        //         Else
        //             Notify schedulers of meta thread move

        volatile bool done = false;
        auto doneWaiter = MakeWaiter([&] {done = true;});
        until.AddWaiter(&doneWaiter);
        while (!done)
        {
        }
    }

    void Release()
    {
        if (--mRefCount == 0)
        {
            delete this;
            MetaScheduler::tCurrentContext = nullptr;
        }
    }

    CRUNCH_ALWAYS_INLINE void WaitFor(IWaitable& waitable, WaitMode waitMode)
    {
        // TODO: Let active scheduler handle the wait if it can
        // TODO: Keep in mind if active scheduler is a fiber scheduler we might come back on a different system thread.. (and this thread might be used for other things.. i.e. waiter must be stack local)
        
        PostWaiter waiter(mWaitSemaphore);
        waitable.AddWaiter(&waiter);
        mWaitSemaphore.SpinWait(waitMode.spinCount);
    }

    CRUNCH_ALWAYS_INLINE void WaitForAll(IWaitable** waitables, std::size_t count, WaitMode waitMode)
    {
        IWaitable** unordered = CRUNCH_STACK_ALLOC_T(IWaitable*, count);
        IWaitable** ordered = CRUNCH_STACK_ALLOC_T(IWaitable*, count);
        std::size_t orderedCount = 0;
        std::size_t unorderedCount = 0;

        for (std::size_t i = 0; i < count; ++i)
        {
            if (waitables[i]->IsOrderDependent())
                ordered[orderedCount++] = waitables[i];
            else
                unordered[unorderedCount++] = waitables[i];
        }

        if (orderedCount != 0)
        {
            std::sort(ordered, ordered + orderedCount);

            // Order dependent doesn't imply fair, so we need to wait for one at a time
            for (std::size_t i = 0; i < orderedCount; ++i)
                WaitFor(*ordered[i], waitMode);
        }

        if (unorderedCount != 0)
        {
            PostWaiter* waiters = CRUNCH_STACK_ALLOC_T(PostWaiter, count);

            for (std::size_t i = 0; i < unorderedCount; ++i)
            {
                ::new (&waiters[i]) PostWaiter(mWaitSemaphore);
                waitables[i]->AddWaiter(&waiters[i]);
            }

            for (std::size_t i = 0; i < unorderedCount; ++i)
                mWaitSemaphore.SpinWait(waitMode.spinCount);
        }
    }

    std::vector<IWaitable*> WaitForAny(IWaitable** waitables, std::size_t count, WaitMode waitMode)
    {
        PostWaiter* waiters = CRUNCH_STACK_ALLOC_T(PostWaiter, count);

        for (std::size_t i = 0; i < count; ++i)
        {
            ::new (&waiters[i]) PostWaiter(mWaitSemaphore);
            waitables[i]->AddWaiter(&waiters[i]);
        }

        mWaitSemaphore.SpinWait(waitMode.spinCount);

        std::vector<IWaitable*> signaled;
        for (std::size_t i = 0; i < count; ++i)
        {
            if (!waitables[i]->RemoveWaiter(&waiters[i]))
                signaled.push_back(waitables[i]);
        }

        // Wait for any waiters that weren't removed. They should be in-flight Notify()
        CRUNCH_ASSERT(signaled.size() >= 1);
        for (std::size_t i = 1; i < signaled.size(); ++i)
            mWaitSemaphore.SpinWait(waitMode.spinCount);

        return signaled;
    }

private:
    struct PostWaiter : Waiter, NonCopyable
    {
        PostWaiter(Detail::SystemSemaphore& semaphore)
            : semaphore(semaphore)
        {}

        virtual void Notify() CRUNCH_OVERRIDE
        {
            semaphore.Post();
        }

        Detail::SystemSemaphore& semaphore;
    };

    MetaScheduler* mOwner;
    uint32 mRefCount;
    Detail::SystemSemaphore mWaitSemaphore;
};


void MetaScheduler::Context::WaitFor(IWaitable& waitable, WaitMode waitMode)
{
    static_cast<ContextImpl*>(this)->WaitFor(waitable, waitMode);
}

void MetaScheduler::Context::WaitForAll(IWaitable** waitables, std::size_t count, WaitMode waitMode)
{
    static_cast<ContextImpl*>(this)->WaitForAll(waitables, count, waitMode);
}

std::vector<IWaitable*> MetaScheduler::Context::WaitForAny(IWaitable** waitables, std::size_t count, WaitMode waitMode)
{
    return static_cast<ContextImpl*>(this)->WaitForAny(waitables, count, waitMode);
}

void MetaScheduler::Context::Run(IWaitable& until)
{
    static_cast<ContextImpl*>(this)->Run(until);
}

void MetaScheduler::Context::Release()
{
    static_cast<ContextImpl*>(this)->Release();
}

CRUNCH_THREAD_LOCAL MetaScheduler::ContextImpl* MetaScheduler::tCurrentContext = NULL;

struct ContextRunState : Waiter
{
    ISchedulerContext* context;

    virtual void Notify() CRUNCH_OVERRIDE
    {
        // Add to active set
        // Wake up scheduler if sleeping
    }
};

class MetaScheduler::MetaThread
{
public:
    MetaThread(MetaThreadConfig const& config)
        : mConfig(config)
    {}

private:
    MetaThreadConfig mConfig;
};

MetaScheduler::MetaScheduler(const SchedulerList& schedulers)
    : mSchedulers(schedulers)
{}

MetaScheduler::~MetaScheduler()
{}

MetaScheduler::MetaThreadHandle MetaScheduler::CreateMetaThread(MetaThreadConfig const& config)
{
    Detail::SystemMutex::ScopedLock lock(mMetaThreadsLock);

    MetaThreadPtr mt(new MetaThread(config));
    mIdleMetaThreads.push_back(std::move(mt));
    return MetaThreadHandle();
}

MetaScheduler::Context& MetaScheduler::AcquireContext()
{
    if (tCurrentContext == nullptr)
    {
        tCurrentContext = new ContextImpl(this);
    }
    else
    {
        CRUNCH_ASSERT_MSG(tCurrentContext->GetOwner() == this, "Context belongs to different MetaScheduler instance");
        tCurrentContext->AddRef();
    }

    return *tCurrentContext;
}

void WaitFor(IWaitable& waitable, WaitMode waitMode)
{
    CRUNCH_ASSERT_MSG(MetaScheduler::tCurrentContext != nullptr, "No context on current thread");
    MetaScheduler::tCurrentContext->WaitFor(waitable, waitMode);
}

void WaitForAll(IWaitable** waitables, std::size_t count, WaitMode waitMode)
{
    CRUNCH_ASSERT_MSG(MetaScheduler::tCurrentContext != nullptr, "No context on current thread");
    MetaScheduler::tCurrentContext->WaitForAll(waitables, count, waitMode);
}

std::vector<IWaitable*> WaitForAny(IWaitable** waitables, std::size_t count, WaitMode waitMode)
{
    CRUNCH_ASSERT_MSG(MetaScheduler::tCurrentContext != nullptr, "No context on current thread");
    return MetaScheduler::tCurrentContext->WaitForAny(waitables, count, waitMode);
}

}}
