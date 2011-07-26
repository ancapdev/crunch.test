// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_CONCURRENCY_WAITABLE_HPP
#define CRUNCH_CONCURRENCY_WAITABLE_HPP

#include "crunch/concurrency/destroyable_waiter.hpp"
#include "crunch/concurrency/self_destroying_waiter.hpp"
#include "crunch/concurrency/wait_mode.hpp"
#include "crunch/containers/small_vector.hpp"

namespace Crunch { namespace Concurrency {

struct IWaitable
{
    template<typename F>
    void AddWaiter(F callback)
    {
        AddWaiter(SelfDestroyingWaiter::Create(callback));
    }

    void AddWaiter(DestroyableWaiter* waiter)
    {
        AddWaiter(static_cast<Waiter*>(waiter));
    }

    void AddWaiter(SelfDestroyingWaiter* waiter)
    {
        AddWaiter(static_cast<Waiter*>(waiter));
    }

    virtual void AddWaiter(Waiter* waiter) = 0;

    virtual bool RemoveWaiter(Waiter* waiter) = 0;

    virtual bool IsOrderDependent() const = 0;

    virtual ~IWaitable() { }
};

/// List of signaled waitables
typedef Containers::SmallVector<IWaitable*, 16> WaitForAnyResult;

void WaitFor(IWaitable& waitable, WaitMode waitMode = WaitMode::Run());
void WaitForAll(IWaitable** waitables, std::size_t count, WaitMode waitMode = WaitMode::Run());
WaitForAnyResult WaitForAny(IWaitable** waitables, std::size_t count, WaitMode waitMode = WaitMode::Run());

}}

#endif
