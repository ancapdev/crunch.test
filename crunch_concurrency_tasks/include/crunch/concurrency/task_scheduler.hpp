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
        Task<F>* task = new Task<F>(*this, std::move(f), dependencyCount);

        // Must get future before adding waiters in case it becomes ready, is stolen, run, and deleted before we return
        auto future = task->GetFuture();

        if (dependencyCount > 0)
        {
            for (uint32 i = 0; i < dependencyCount; ++i)
                dependencies[i]->AddWaiter([=] { task->NotifyDependencyReady(); });
        }
        else
        {
            mTasks.push_back(task);
        }

        return future;
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
void Task<F>::Dispatch(ResultClassFutureGeneric)
{
    Future<ResultType> result = mFunctor();

    // Create continuation dependent on result

    FutureDataType* fd = mFutureData;
    auto contFunc = [=] {
        fd->Set(result.Get());
        Detail::Release(fd);
    };

    auto contTask = new Task<decltype(contFunc)>(mOwner, std::move(contFunc), 1, 1);

    result.AddWaiter([=] { contTask->NotifyDependencyReady(); });

    delete this;
}

template<typename F>
void Task<F>::Dispatch(ResultClassFutureVoid)
{
    Future<void> result = mFunctor();

    // Create continuation dependent on result

    Detail::FutureData<void>* fd = mFutureData;
    auto contFunc = [=] {
        fd->Set();
        Detail::Release(fd);
    };

    // Nothing will be depending on this continuation task, so can take some shortcuts:
    // - No FutureData
    auto contTask = new Task<decltype(contFunc)>(mOwner, std::move(contFunc), 1, 1);

    result.AddWaiter([=] { contTask->NotifyDependencyReady(); });

    delete this;
}


}}

#endif
