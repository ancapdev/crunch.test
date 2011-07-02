// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/concurrency/thread.hpp"
#include "crunch/concurrency/exceptions.hpp"
#include "crunch/concurrency/thread_local.hpp"

#include "crunch/base/assert.hpp"

#include <exception>
#include <memory>

#include <windows.h>

namespace Crunch { namespace Concurrency {

struct Thread::Data
{
    Data(std::function<void ()>&& f)
        : handle(NULL)
        , id(0)
        , userEntryPoint(std::move(f))
        , cancellationRequested(false)
        , cancellationEnabled(true)
        , canceled(false)
    {}

    static DWORD WINAPI EntryPoint(void* argument)
    {
        DataPtr data = reinterpret_cast<Data*>(argument)->self;
        data->self.reset();

        tCurrent = data.get();

        try
        {
            data->userEntryPoint();
        }
        catch (...)
        {
            std::terminate();
        }

        return 0;
    }

    DataPtr self;
    HANDLE handle;
    DWORD id;
    std::function<void ()> userEntryPoint;
    bool cancellationRequested;
    bool cancellationEnabled;
    bool canceled;

    static CRUNCH_THREAD_LOCAL Thread::Data* tCurrent;
};

CRUNCH_THREAD_LOCAL Thread::Data* Thread::Data::tCurrent = NULL;

Thread::~Thread()
{
    if (IsJoinable())
    {
        Cancel();
        Detach();
    }
}

void Thread::Cancel()
{
    if (mData)
        mData->cancellationRequested = true;
}

bool Thread::IsCancellationRequested() const
{
    return mData && mData->cancellationRequested && !mData->canceled;
}

ThreadId Thread::GetId() const
{
    if (mData)
        return mData->id;
    else
        return ThreadId();
}

void Thread::Create(std::function<void ()>&& f)
{
    mData.reset(new Data(std::move(f)));
    mData->self = mData;

    mData->handle = ::CreateThread(NULL, 0, &Data::EntryPoint, mData.get(), 0, &mData->id);
    if (mData->handle == NULL)
    {
        mData->self.reset();
        mData.reset();
        throw ThreadResourceError();
    }
}

void Thread::Detach()
{
    if (mData)
    {
        CloseHandle(mData->handle);
        mData.reset();
    }
}

void Thread::Join()
{
    ThreadCancellationPoint();

    if (mData)
    {
        CRUNCH_ASSERT_ALWAYS(WaitForSingleObject(mData->handle, INFINITE) == WAIT_OBJECT_0);
        CloseHandle(mData->handle);
        mData.reset();
    }
}

ThreadId GetThreadId()
{
    return ::GetCurrentThreadId();
}

void SetThreadCancellationPolicy(bool enableCancellation)
{
    if (Thread::Data::tCurrent)
        Thread::Data::tCurrent->cancellationEnabled = enableCancellation;
} 

void ThreadCancellationPoint()
{
    if (Thread::Data::tCurrent &&
        Thread::Data::tCurrent->cancellationRequested &&
        !Thread::Data::tCurrent->canceled)
    {
        Thread::Data::tCurrent->canceled = true;
        throw ThreadCanceled();
    }
}

bool IsThreadCancellationEnabled()
{
    return Thread::Data::tCurrent && Thread::Data::tCurrent->cancellationEnabled;
}

bool IsThreadCancellationRequested()
{
    return Thread::Data::tCurrent && Thread::Data::tCurrent->cancellationRequested;
}

}}
