// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_CONCURRENCY_TASK_SCHEDULER_HPP
#define CRUNCH_CONCURRENCY_TASK_SCHEDULER_HPP

#include "crunch/base/stdint.hpp"
#include "crunch/base/noncopyable.hpp"
#include "crunch/concurrency/future.hpp"
#include "crunch/concurrency/task.hpp"

#include <deque>
#include <functional>

namespace Crunch { namespace Concurrency {


// TODO: store continuation size hint thread local per F type 
//       would enable over-allocation on initial task to avoid further allocations for continuations
//       only necessary when return type is void or Future<T>, i.e., continuable
class TaskScheduler : NonCopyable
{
public:
    // TaskImpl should be detail
    template<typename F>
    auto Add (F f) -> typename Future<typename TaskTraits<F>::ResultType>
    {
        return Add(f, nullptr, 0);
    }

    template<typename F>
    auto Add (F f, IWaitable** dependencies, uint32 dependencyCount) -> Future<typename TaskTraits<F>::ResultType>
    {
        typedef Future<typename TaskTraits<F>::ResultType> FutureType;
        typedef typename FutureType::DataType FutureDataType;
        typedef typename FutureType::DataPtr FutureDataPtr;

        FutureDataType* futureData = new FutureDataType(2);
        Task<F>* task = new Task<F>(*this, std::move(f), futureData, dependencyCount);

        if (dependencyCount > 0)
        {
            for (uint32 i = 0; i < dependencyCount; ++i)
                dependencies[i]->AddWaiter([=] { task->NotifyDependencyReady(); });
        }
        else
        {
            mTasks.push_back(task);
        }

        return FutureType(FutureDataPtr(futureData, false));
    }

    void RunAll()
    {
        while (!mTasks.empty())
        {
            TaskBase* t = mTasks.back();
            mTasks.pop_back();
            t->Dispatch();
        }
    }

private:
    friend class TaskBase;

    // Mock up to test interface
    std::deque<TaskBase*> mTasks;
};

inline void TaskBase::NotifyDependencyReady()
{
    if (1 == mBarrierCount.Decrement())
        mOwner.mTasks.push_back(this);
}

template<typename F>
void Task<F>::Dispatch(ResultClassFuture)
{
    auto result = mFunctor();

    // Create continuation dependent on the completion of the returned Future
    auto futureData = mFutureData;
    TaskScheduler& owner = mOwner;

    // Get value from result
    auto contFunc = [=] () -> ResultType { return result.Get(); };
    typedef Task<decltype(contFunc)> ContTaskType;

    ContTaskType* contTask;
    if (sizeof(Task<F>) < sizeof(ContTaskType))
    {
        // Create new allocation
        delete this;
        contTask = new ContTaskType(owner, std::move(contFunc), futureData, 1);
    }
    else
    {
        // Reuse current allocation
        uint32 const allocSize = mAllocationSize;
        this->~Task<F>();
        contTask = new (this) ContTaskType(owner, std::move(contFunc), futureData, 1, allocSize);
    }

    result.AddWaiter([=] { contTask->NotifyDependencyReady(); });
}

}}

#endif
