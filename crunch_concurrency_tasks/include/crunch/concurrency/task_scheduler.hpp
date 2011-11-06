// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_CONCURRENCY_TASK_SCHEDULER_HPP
#define CRUNCH_CONCURRENCY_TASK_SCHEDULER_HPP

#include "crunch/base/stdint.hpp"
#include "crunch/concurrency/future.hpp"
#include "crunch/concurrency/task.hpp"

#include <deque>
#include <functional>

namespace Crunch { namespace Concurrency {

template<typename R>
struct UnwrapResultType
{
    typedef R Type;
};

template<typename R>
struct UnwrapResultType<Future<R>>
{
    typedef R Type;
};

template<typename F>
struct TaskTraits
{
    // MSVC 10 std::result_of is broken
    typedef decltype((*(F*)(0))()) RawResultType;
    typedef typename UnwrapResultType<RawResultType>::Type ResultType;
    typedef Future<ResultType> FutureType;
    typedef TaskImpl<F, ResultType> TaskType;
};

class TaskScheduler
{
public:
    // Maybe needs to return Task rather than Future in order to provide a richer interface
    // like cancellation:
    /*
    class Task<T>
    {
    public:
        Future<T> GetResult();
        void Cancel();
    }
    */
    // Cancellation could be provided by passing a cancellation token to the Add() method, without changin the result Future object.

    // TaskImpl should be detail
    template<typename F>
    auto Add (F f) -> typename TaskTraits<F>::FutureType
    {
        typedef typename TaskTraits<F>::FutureType FutureType;
        typedef typename TaskTraits<F>::TaskType TaskType;

        TaskType* task = new TaskType(std::move(f));
        mTasks.push_back(task);
        return FutureType(typename FutureType::DataPtr(task));
    }

    template<typename F>
    auto Add (F f, IWaitable** dependencies, uint32 dependencyCount) -> typename TaskTraits<F>::FutureType
    {
        (void)dependencies;
        (void)dependencyCount;
        return Add(f);
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
