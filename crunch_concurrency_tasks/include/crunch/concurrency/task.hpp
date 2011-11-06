// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_CONCURRENCY_TASK_HPP
#define CRUNCH_CONCURRENCY_TASK_HPP

#include "crunch/base/align.hpp"
#include "crunch/base/override.hpp"
#include "crunch/base/stdint.hpp"

#include "crunch/concurrency/waitable.hpp"
#include "crunch/concurrency/future.hpp"

#include <type_traits>

namespace Crunch { namespace Concurrency {


class Task
{
public:

protected:
    friend class TaskScheduler;

    typedef void (*DispatchFunction)(Task*);

    Task(uint32 barrierCount)
        : mBarrierCount(barrierCount, MEMORY_ORDER_RELEASE)
    {}

    DispatchFunction mDispatcher;
    Atomic<uint32> mBarrierCount;
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

template<typename ResultType, typename ReturnType>
struct DispatchHelper
{
    template<typename F>
    static void Dispatch(const F& f, Detail::FutureData<ResultType>& fd)
    {
        fd.Set(f());
    }
};

template<>
struct DispatchHelper<void, void>
{
    template<typename F>
    static void Dispatch(const F& f, Detail::FutureData<void>& fd)
    {
        f();
        fd.Set();
    }
};

template<typename R>
struct DispatchHelper<R, Future<R>>
{
    template<typename F>
    static void Dispatch(const F& f, Detail::FutureData<R>&)
    {
        Future<R> result = f();
        // Create continuation dependent on result
    }
};

template<typename F, typename R>
class TaskImpl : public Task, public Detail::FutureData<R>
{
public:
    TaskImpl(F&& f, uint32 barrierCount = 0)
        : Task(barrierCount)
    {
        Detail::FutureDataBase::mRefCount.Store(1, MEMORY_ORDER_RELAXED);
        mDispatcher = &TaskImpl<F, R>::Dispatch;
        mAllocationSize = sizeof(TaskImpl<F, R>);
        new (&mFunctorStorage) F(f);
    }

    static void Dispatch(Task* task)
    {
        TaskImpl<F, R>* this_ = static_cast<TaskImpl<F, R>*>(task);
        F* f = reinterpret_cast<F*>(&this_->mFunctorStorage);
        DispatchHelper<R, decltype((*f)())>::Dispatch(*f, *this_);
        Release(this_);
    }

    virtual void Destroy() CRUNCH_OVERRIDE
    {
        delete this;
    }

    typedef typename std::aligned_storage<sizeof(F), std::alignment_of<F>::value>::type FunctorStorageType;

    FunctorStorageType mFunctorStorage;
};

/*
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
*/

}}

#endif
