#ifndef CRUNCH_CONCURRENCY_META_SCHEDULER_HPP
#define CRUNCH_CONCURRENCY_META_SCHEDULER_HPP

#include "crunch/base/stdint.hpp"
#include "crunch/concurrency/scheduler.hpp"
#include "crunch/concurrency/thread_local.hpp"
#include "crunch/concurrency/waitable.hpp"
#include "crunch/concurrency/detail/system_event.hpp"
#include "crunch/concurrency/detail/system_mutex.hpp"

#include <memory>
#include <vector>

namespace Crunch { namespace Concurrency {

// Very simple scheduler to run multiple cooperative schedulers. E.g.,
// - task scheduler
// - io_service scheduler
// - fiber / coroutine scheduler
class MetaScheduler
{
public:
    typedef std::shared_ptr<IScheduler> SchedulerPtr;
    typedef std::vector<SchedulerPtr> SchedulerList;

    MetaScheduler(SchedulerList const& schedulers);
    ~MetaScheduler();

    class ThreadConfig
    {
    public:
        ThreadConfig() : mAffinity(0xfffffffful) {}

        void SetAffinity(uint32 affinity) { mAffinity = affinity; }
        uint32 GetAffinity() const { return mAffinity; }

    private:
        uint32 mAffinity;
    };
    
    void Join(ThreadConfig const& config);

    void Leave();

    void Run(IWaitable& until);

    /*
    // Run the scheduler on this thread.
    // TODO: mask for which types of schedulers to run? or simply have multiple meta schedulers for that purpose?
    // - Could have a two way affinity mask, one for the scheduler and one for the thread. If their conjunction is non-zero run the scheduler
    // - Should maybe notify a scheduler that it is not running on a particular thread. E.g., to orphan work to other threads immediately.
    void Run();

    // Interrupt all threads running the scheduler
    void InterruptAll();

    // Interrupt the current thread's scheduler.
    void InterruptCurrent();
    */

private:
    friend void WaitFor(IWaitable&, WaitMode);
    friend void WaitForAll(IWaitable**, std::size_t, WaitMode);
    friend void WaitForAny(IWaitable**, std::size_t, WaitMode);

    class Context;
    typedef std::unique_ptr<Context> ContextPtr;

    typedef std::vector<ContextPtr> ContextList;

    SchedulerList mSchedulers;
    Detail::SystemMutex mSchedulersLock;

    ContextList mContexts;
    Detail::SystemMutex mContextsLock;

    static CRUNCH_THREAD_LOCAL Context* tCurrentContext;

    // For use in threads that haven't joined the scheduler
    static Detail::SystemMutex sSharedEventLock;
    static Detail::SystemEvent sSharedEvent;
};

}}

#endif
