#ifndef CRUNCH_CONCURRENCY_META_SCHEDULER_HPP
#define CRUNCH_CONCURRENCY_META_SCHEDULER_HPP

#include "crunch/concurrency/scheduler.hpp"
#include "crunch/concurrency/thread_local.hpp"
#include "crunch/concurrency/waitable.hpp"

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include <vector>

namespace Crunch { namespace Concurrency {

// Very simple scheduler to run multiple cooperative schedulers. E.g.,
// - task scheduler
// - io_service scheduler
// - fiber / coroutine scheduler
class MetaScheduler
{
public:
    typedef boost::shared_ptr<IScheduler> SchedulerPtr;
    typedef std::vector<SchedulerPtr> SchedulerList;

    MetaScheduler(SchedulerList const& schedulers);

    class ThreadConfig
    {
    public:
        ThreadConfig() : mAffinity(0xfffffffful) {}

        void SetAffinity(boost::uint32_t affinity) { mAffinity = affinity; }
        boost::uint32_t GetAffinity() const { return mAffinity; }

    private:
        boost::uint32_t mAffinity;
    };
    
    void Join(ThreadConfig const& config);

    void Leave();

    void Run(IWaitable& until);

    enum WaitMode
    {
        WAIT_MODE_POLL,
        WAIT_MODE_YIELD_PREEMTIVE,
        WAIT_MODE_YIELD_COOPERATIVE
    };

    static void WaitForAll(IWaitable** waitables, std::size_t count, WaitMode waitMode = WAIT_MODE_YIELD_COOPERATIVE);
    static void WaitForAny(IWaitable** waitables, std::size_t count, WaitMode waitMode = WAIT_MODE_YIELD_COOPERATIVE);


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
    class Context;

    typedef boost::ptr_vector<Context> ContextList;

    SchedulerList mSchedulers;
    boost::mutex mSchedulersLock;

    ContextList mContexts;
    boost::mutex mContextsLock;

    CRUNCH_THREAD_LOCAL static Context* tCurrentContext;
};

}}

#endif
