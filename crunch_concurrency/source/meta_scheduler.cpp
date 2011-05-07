#include "crunch/concurrency/meta_scheduler.hpp"

#include <boost/foreach.hpp>

namespace Crunch { namespace Concurrency {

struct ContextRunState : Waiter
{
    ISchedulerContext* context;

    virtual void Wakeup()
    {
        // Add to active set
        // Wake up scheduler if sleeping
    }
};

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

}}
