// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_CONCURRENCY_TASK_SCHEDULER_HPP
#define CRUNCH_CONCURRENCY_TASK_SCHEDULER_HPP

#include "crunch/base/stdint.hpp"
#include "crunch/concurrency/future.hpp"
#include "crunch/concurrency/task.hpp"

#include <deque>

namespace Crunch { namespace Concurrency {



class TaskScheduler
{
public:
    // Maybe needs to return Task rather than Future in order to richer interface
    // like cancellation:
    /*
    class Task<T>
    {
    public:
        Future<T> GetResult();
        void Cancel();
    }
    */
    // TaskImpl should be detail
    template<typename F>
    auto Add (F f, IWaitable** dependencies, uint32 dependencyCount) -> Future<decltype(f())>
    {
        (void)dependencies;
        (void)dependencyCount;
        typedef decltype(f()) ResultType;
        typedef Future<ResultType> FutureType;
        typedef TaskImpl<F, ResultType> TaskType;
        TaskType* task = new TaskType(std::move(f));
        mTasks.push_back(task);
        return FutureType(FutureType::DataPtr(task));
    }

    void RunAll()
    {
        while (!mTasks.empty())
        {
            mTasks.back()->mDispatcher(mTasks.back());
            mTasks.pop_back();
        }
    }

private:
    // Mock up to test interface
    std::deque<Task*> mTasks;
};

}}

#endif
