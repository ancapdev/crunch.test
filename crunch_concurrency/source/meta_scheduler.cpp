// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/concurrency/meta_scheduler.hpp"

#include "crunch/base/assert.hpp"
#include "crunch/base/noncopyable.hpp"
#include "crunch/base/override.hpp"
#include "crunch/base/stack_alloc.hpp"
#include "crunch/concurrency/event.hpp"
#include "crunch/concurrency/yield.hpp"
#include "crunch/concurrency/detail/system_semaphore.hpp"

#include <algorithm>

namespace Crunch { namespace Concurrency {

class MetaScheduler::Context : NonCopyable, Waiter
{
public:
    Context(MetaScheduler& owner)
        : mOwner(owner)
        , mWaitSemaphore(0)
        , mWaitSpinCount(400)
        , mWaitState(WAIT_STATE_SPINNING)
    {}

    static int const WAIT_STATE_SPINNING = 0;
    static int const WAIT_STATE_WAITING = 1;
    static int const WAIT_STATE_DONE = 2;

    void Notify() CRUNCH_OVERRIDE
    {
        int s = WAIT_STATE_SPINNING;

        // Try to unblock from spinning
        if (mWaitState.CompareAndSwap(WAIT_STATE_DONE, s))
            return;

        // Must be waiting
        CRUNCH_ASSERT(s == WAIT_STATE_WAITING);
        mWaitSemaphore.Post();
    }

    void WaitFor(IWaitable& waitable)
    {
        // TODO: Let active scheduler handle the wait if it can
        // TODO: Keep in mind if active scheduler is a fiber scheduler we might come back on a different system thread.. (and this thread might be used for other things.. i.e. waiter must be stack local)

        mWaitState.Store(WAIT_STATE_SPINNING, MEMORY_ORDER_RELAXED);

        waitable.AddWaiter(this);

        uint32 spinLeft = mWaitSpinCount;
        while (spinLeft--)
        {
            if (mWaitState.Load(MEMORY_ORDER_RELAXED) == WAIT_STATE_DONE)
                return;

            CRUNCH_PAUSE();
        }

        int s = WAIT_STATE_SPINNING;
        if (!mWaitState.CompareAndSwap(WAIT_STATE_WAITING, s))
            return; // Must have completed

        mWaitSemaphore.Wait();
    }

private:
    MetaScheduler& mOwner;
    Detail::SystemSemaphore mWaitSemaphore;
    uint32 mWaitSpinCount;
    Atomic<int> mWaitState;
};

CRUNCH_THREAD_LOCAL MetaScheduler::Context* MetaScheduler::tCurrentContext = NULL;
Detail::SystemMutex MetaScheduler::sSharedEventLock;
Detail::SystemEvent MetaScheduler::sSharedEvent;

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

void MetaScheduler::Join()
{
    CRUNCH_ASSERT_ALWAYS(tCurrentContext == nullptr);
    tCurrentContext = new Context(*this);
}

void MetaScheduler::Leave()
{
    CRUNCH_ASSERT_ALWAYS(tCurrentContext != nullptr);
    delete tCurrentContext;
    tCurrentContext = nullptr;
}

void MetaScheduler::Run(IWaitable& until)
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

void WaitFor(IWaitable& waitable, WaitMode)
{
    MetaScheduler::Context* context = MetaScheduler::tCurrentContext;

    if (context)
        context->WaitFor(waitable);
    else
    {
        auto waiter = MakeWaiter([&] { MetaScheduler::sSharedEvent.Set(); });
        Detail::SystemMutex::ScopedLock lock(MetaScheduler::sSharedEventLock);
     
        MetaScheduler::sSharedEvent.Reset();
        waitable.AddWaiter(&waiter);
        MetaScheduler::sSharedEvent.Wait();
    }
}

void WaitForAll(IWaitable** waitables, std::size_t count, WaitMode)
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
        std::sort(ordered, ordered + orderedCount);

    MetaScheduler::Context* context = MetaScheduler::tCurrentContext;

    if (context)
    {
        // Order dependent doesn't imply fair, so we need to wait for one at a time
        for (std::size_t i = 0; i < orderedCount; ++i)
            context->WaitFor(*ordered[i]);

        // TODO: could add all waiters in one go and do only one wait
        for (std::size_t i = 0; i < unorderedCount; ++i)
            context->WaitFor(*unordered[i]);
    }
    else
    {
        auto waiter = MakeWaiter([&] { MetaScheduler::sSharedEvent.Set(); });
        auto waitHelper = [&] (IWaitable& waitable) {
            MetaScheduler::sSharedEvent.Reset();
            waitable.AddWaiter(&waiter);
            MetaScheduler::sSharedEvent.Wait();
        };
            
        Detail::SystemMutex::ScopedLock lock(MetaScheduler::sSharedEventLock);

        for (std::size_t i = 0; i < orderedCount; ++i)
            waitHelper(*ordered[i]);

        for (std::size_t i = 0; i < unorderedCount; ++i)
            waitHelper(*unordered[i]);
    }
}

void WaitForAny(IWaitable** waitables, std::size_t count, WaitMode)
{
    struct WaiterHelper : Waiter, NonCopyable
    {
        WaiterHelper(Event& event)
            : event(event)
        {}

        virtual void Notify() CRUNCH_OVERRIDE
        {
            event.Set();
        }

        Event& event;
    };

    WaiterHelper* waiters = CRUNCH_STACK_ALLOC_T(WaiterHelper, count);

    Event event(false);

    for (std::size_t i = 0; i < count; ++i)
    {
        ::new (&waiters[i]) WaiterHelper(event);
        waitables[i]->AddWaiter(&waiters[i]);
    }

    MetaScheduler::Context* context = MetaScheduler::tCurrentContext;
    if (context)
    {
        context->WaitFor(event);
    }
    else
    {
        auto waiter = MakeWaiter([&] { MetaScheduler::sSharedEvent.Set(); });

        Detail::SystemMutex::ScopedLock lock(MetaScheduler::sSharedEventLock);
        MetaScheduler::sSharedEvent.Reset();
        event.AddWaiter(&waiter);
        MetaScheduler::sSharedEvent.Wait();
    }

    for (std::size_t i = 0; i < count; ++i)
    {
        waitables[i]->RemoveWaiter(&waiters[i]);
    }
}

}}
