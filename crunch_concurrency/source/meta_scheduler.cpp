#include "crunch/concurrency/meta_scheduler.hpp"

#include "crunch/base/assert.hpp"
#include "crunch/base/stack_alloc.hpp"
#include "crunch/concurrency/event.hpp"

#include <algorithm>

#if defined (CRUNCH_PLATFORM_WIN32)
#   include <windows.h>
#endif

namespace Crunch { namespace Concurrency {

class MetaScheduler::Context : Waiter
{
public:
    Context(MetaScheduler& owner)
        : mOwner(owner)
#if defined (CRUNCH_PLATFORM_WIN32)
        , mEvent(CreateEvent(NULL, TRUE, FALSE, NULL))
#endif
    {
#if defined (CRUNCH_PLATFORM_WIN32)
        CRUNCH_ASSERT_ALWAYS(mEvent != NULL);
#endif
    }

    ~Context()
    {
#if defined (CRUNCH_PLATFORM_WIN32)
        CloseHandle(mEvent);
#endif
    }

    void WaitFor(IWaitable& waitable)
    {
        // TODO: Let active scheduler handle the wait if it can
        // TODO: Keep in mind if active scheduler is a fiber scheduler we might come back on a different system thread.. (and this thread might be used for other things.. i.e. waiter must be stack local)
#if defined (CRUNCH_PLATFORM_WIN32)
        ResetEvent(mEvent);
#endif
        waitable.AddWaiter(this);
#if defined (CRUNCH_PLATFORM_WIN32)
        WaitForSingleObject(mEvent, INFINITE);
#endif
    }

    virtual void Wakeup()
    {
#if defined (CRUNCH_PLATFORM_WIN32)
        SetEvent(mEvent);
#endif
    }

private:
    MetaScheduler& mOwner;

#if  defined (CRUNCH_PLATFORM_WIN32)
    HANDLE mEvent;
#endif
};

void MetaScheduler::WaitForAll(IWaitable** waitables, std::size_t count, WaitMode waitMode)
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

    Context& context = *tCurrentContext;

    if (orderedCount != 0)
    {
        std::sort(ordered, ordered + orderedCount);

        for (std::size_t i = 0; i < orderedCount; ++i)
        {
            // Order dependent doesn't imply fair, so we need to wait for one at a time
            context.WaitFor(*ordered[i]);
        }
    }

    // TODO: could add all waiters in one go and do only one wait
    for (std::size_t i = 0; i < unorderedCount; ++i)
    {
        context.WaitFor(*unordered[i]);
    }
}

void MetaScheduler::WaitForAny(IWaitable** waitables, std::size_t count, WaitMode waitMode)
{
    struct WaiterHelper : Waiter
    {
        virtual void Wakeup()
        {
            event->Set();
        }

        Event* event;
    };

    WaiterHelper* waiters = CRUNCH_STACK_ALLOC_T(WaiterHelper, count);

    Event event(false);

    for (std::size_t i = 0; i < count; ++i)
    {
        waiters[i].event = &event;
        waitables[i]->AddWaiter(&waiters[i]);
    }

    tCurrentContext->WaitFor(event);

    for (std::size_t i = 0; i < count; ++i)
    {
        waitables[i]->RemoveWaiter(&waiters[i]);
    }
}


CRUNCH_THREAD_LOCAL MetaScheduler::Context* MetaScheduler::tCurrentContext = NULL;

struct ContextRunState : Waiter
{
    ISchedulerContext* context;

    virtual void Wakeup()
    {
        // Add to active set
        // Wake up scheduler if sleeping
    }
};

void MetaScheduler::Join(ThreadConfig const& config)
{
    CRUNCH_ASSERT_ALWAYS(tCurrentContext == nullptr);

    // Set up thread specific data
    ContextPtr context(new Context(*this));
    tCurrentContext = context.get();
    mContexts.push_back(std::move(context));
}

void MetaScheduler::Leave()
{
    CRUNCH_ASSERT_ALWAYS(tCurrentContext != nullptr);

    tCurrentContext = NULL;
}

void MetaScheduler::Run(IWaitable& until)
{
}

/*
void MetaScheduler::Run()
{
    typedef std::vector<ContextRunState> ContextList;

    ContextList runStates;
    {
        const boost::mutex::scoped_lock lock(mSchedulersLock);

        BOOST_FOREACH(const SchedulerPtr& scheduler, mSchedulers)
        {
            ContextRunState runState;
            runState.context = &scheduler->GetContext();
            runStates.push_back(runState);
        }
    }

    // Check if scheduler master list has changed
    // - Add / Remove schedulers as needed
    // - Should probably also hold copy of scheduler ptr as long as we use context
    for (;;)
    {
        for (ContextList::iterator it = runStates.begin();
            it != runStates.end();)
        {
            ISchedulerContext& context = *it->context;
            const ISchedulerContext::State state = context.RunOne();
            if (state == ISchedulerContext::STATE_IDLE)
            {
                it = runStates.erase(it);
                // idleRunStates.push_back(&context);
                context.GetHasWorkCondition().AddWaiter(&*it);
            }
            else
            {
                ++it;
            }
        }

        // If active list empty, go idle
    }
}
}*/

}}
