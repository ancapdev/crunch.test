#ifndef CRUNCH_CONCURRENCY_TASK_HPP
#define CRUNCH_CONCURRENCY_TASK_HPP

#include "crunch/base/align.hpp"
#include "crunch/base/override.hpp"
#include "crunch/base/stdint.hpp"

#include "crunch/concurrency/waitable.hpp"
#include "crunch/concurrency/detail/future_data.hpp"

#include <type_traits>

namespace Crunch { namespace Concurrency {


class Task
{
public:


private:
    typedef void (*DispatchFunction)(Task*);

    DispatchFunction mDispatcher;
    uint32 mBarrierCount;
    uint32 mAllocationSize;
    
    struct EmbeddedWaiter : Waiter
    {
        EmbeddedWaiter(Task* task) : task(task) {}

        virtual void Notify() CRUNCH_OVERRIDE
        {
            // Dec barrier and add to scheduler if 0
        }

        Task* task;
    };

    struct HeapWaiter : Waiter
    {
        HeapWaiter(Task* task) : task(task) {}

        virtual void Notify() CRUNCH_OVERRIDE
        {
            // Dec barrier and add to scheduler if 0

            delete this;
        }

        Task* task;
    };
};

template<typename F, typename R>
class TaskImpl : public Task
{
    static void Dispatch(Task* task)
    {
    }

    typedef typename std::aligned_storage<sizeof(F), std::alignment_of<F>::value>::type FunctorStorageType;

    Detail::FutureData<R> mFutureData;
    FunctorStorageType mFunctorStorage;
    EmbeddedWaiter mWaiters[];
};

// For use when the contination doesn't fit in the original tasks allocation
template<typename F, typename R>
class ContinuationImpl : public Task
{
    static void Dispatch(Task* task)
    {
        // Must delete the task because it's not part of the FutureData allocation
    }

    typedef typename std::aligned_storage<sizeof(F), std::alignment_of<F>::value>::type FunctorStorageType;

    Detail::FutureData<R>* mFutureData;
    FunctorStorageType mFunctorStorage;
    EmbeddedWaiter mWaiters[];
};

}}

#endif
