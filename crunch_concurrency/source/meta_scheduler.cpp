#include "crunch/concurrency/meta_scheduler.hpp"

#include "crunch/base/assert.hpp"
#include "crunch/base/override.hpp"
#include "crunch/base/stack_alloc.hpp"
#include "crunch/concurrency/event.hpp"

#include <algorithm>

namespace Crunch { namespace Concurrency {

class MetaScheduler::Context
{
public:
    Context(MetaScheduler& owner)
        : mOwner(owner)
        , mEvent(false)
    {}

    void WaitFor(IWaitable& waitable)
    {
        // TODO: Let active scheduler handle the wait if it can
        // TODO: Keep in mind if active scheduler is a fiber scheduler we might come back on a different system thread.. (and this thread might be used for other things.. i.e. waiter must be stack local)

        auto waiter = MakeWaiter([&] { mEvent.Set(); });

        mEvent.Reset();
        waitable.AddWaiter(&waiter);
        mEvent.Wait();
    }

private:
    MetaScheduler& mOwner;
    Detail::SystemEvent mEvent;
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

MetaScheduler::MetaScheduler(const SchedulerList& schedulers)
    : mSchedulers(schedulers)
{}

MetaScheduler::~MetaScheduler()
{}

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

void WaitFor(IWaitable& waitable, WaitMode waitMode)
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

void WaitForAll(IWaitable** waitables, std::size_t count, WaitMode waitMode)
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

void WaitForAny(IWaitable** waitables, std::size_t count, WaitMode waitMode)
{
    struct WaiterHelper : Waiter
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
