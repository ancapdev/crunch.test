// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/concurrency/task_scheduler.hpp"

namespace Crunch { namespace Concurrency {

void WorkStealingTaskQueue::PushBack(TaskBase* task)
{
    Detail::SystemMutex::ScopedLock lock(mMutex);
    mTasks.push_back(task);
}

TaskBase* WorkStealingTaskQueue::PopBack()
{
    Detail::SystemMutex::ScopedLock lock(mMutex);

    if (mTasks.empty())
        return nullptr;

    TaskBase* task = mTasks.back();
    mTasks.pop_back();
    return task;
}

TaskBase* WorkStealingTaskQueue::StealFront()
{
    Detail::SystemMutex::ScopedLock lock(mMutex);

    if (mTasks.empty())
        return nullptr;

    TaskBase* task = mTasks.front();
    mTasks.pop_front();
    return task;
}


CRUNCH_THREAD_LOCAL TaskScheduler::Context* TaskScheduler::tContext = nullptr;

#if defined (CRUNCH_COMPILER_MSVC)
#   pragma warning (push)
#   pragma warning (disable : 4355) // 'this' used in base member initializer list
#endif

TaskScheduler::TaskScheduler()
    : mSharedContext(*this)
{}

#if defined (CRUNCH_COMPILER_MSVC)
#   pragma warning (pop)
#endif

void TaskScheduler::Enter()
{
    CRUNCH_ASSERT_ALWAYS(tContext == nullptr);
    tContext = new Context(*this);
}

void TaskScheduler::Leave()
{
    CRUNCH_ASSERT_ALWAYS(tContext != nullptr);
    delete tContext;
    tContext = nullptr;
}

void TaskScheduler::Run()
{
    CRUNCH_ASSERT_ALWAYS(tContext != nullptr);
    tContext->RunAll();
}

TaskScheduler::Context::Context(TaskScheduler& owner)
    : mOwner(owner)
{}

void TaskScheduler::Context::RunAll()
{
    while (TaskBase* task = mTasks.PopBack())
        task->Dispatch();
}

}}
