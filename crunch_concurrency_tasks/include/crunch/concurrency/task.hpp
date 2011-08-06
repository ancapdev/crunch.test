// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

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

protected:
    friend class TaskScheduler;

    typedef void (*DispatchFunction)(Task*);

    DispatchFunction mDispatcher;
    uint32 mBarrierCount;
    uint32 mAllocationSize;
    
    /*
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
    */
};

template<typename F, typename R>
class TaskImpl : public Task, Detail::FutureData<R>
{
public:
    TaskImpl(F&& f)
    {
        mRefCount.Store(1, MEMORY_ORDER_RELAXED);
        mDispatcher = &TaskImpl<F, R>::Dispatch;
        mBarrierCount = 0;
        mAllocationSize = sizeof(TaskImpl<F, R>);
        new (&mFunctorStorage) F(f);
    }

    static void Dispatch(Task* task)
    {
        TaskImpl<F, R>* this_ = static_cast<TaskImpl<F, R>*>(task);
        F* f = reinterpret_cast<F*>(&this_->mFunctorStorage);
        this_->Set((*f)());
        Release(this_);
    }

    virtual void Destroy() CRUNCH_OVERRIDE
    {
        delete this;
    }

    typedef typename std::aligned_storage<sizeof(F), std::alignment_of<F>::value>::type FunctorStorageType;

    FunctorStorageType mFunctorStorage;
};

// For use when the continuation doesn't fit in the original tasks allocation
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
};

}}

#endif
