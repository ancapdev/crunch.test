// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/concurrency/thread.hpp"
#include "crunch/base/assert.hpp"

#include <exception>
#include <memory>

namespace Crunch { namespace Concurrency {

namespace
{
    void* ThreadEntryPoint(void* argument)
    {
        typedef std::function<void ()> F;
        std::unique_ptr<F> f(reinterpret_cast<F*>(argument));

        try
        {
            (*f)();
        }
        catch (...)
        {
            std::terminate();
        }

        return nullptr;
    }
}

void Thread::Create(std::function<void ()>&& f)
{
    std::function<void ()>* fp(new std::function<void ()>(std::move(f)));

    int result = pthread_create(&mHandle, nullptr, &ThreadEntryPoint, fp);

    if (result != 0)
    {
        delete fp;
        // TODO: throw thread_resource_error
    }
    else
    {
        mInitialized = true;
    }
}

void Thread::Join()
{
    CRUNCH_ASSERT_ALWAYS(IsJoinable());
    CRUNCH_ASSERT_ALWAYS(pthread_join(mHandle, nullptr) == 0);
    mInitialized = false;
}

void Thread::Detach()
{
    CRUNCH_ASSERT_ALWAYS(IsJoinable());
    CRUNCH_ASSERT_ALWAYS(pthread_detach(mHandle) == 0);
    mInitialized = false;
}

}}
