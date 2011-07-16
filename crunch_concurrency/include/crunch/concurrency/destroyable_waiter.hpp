// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_CONCURRENCY_DESTROYABLE_WAITER_HPP
#define CRUNCH_CONCURRENCY_DESTROYABLE_WAITER_HPP

#include "crunch/concurrency/waiter.hpp"

namespace Crunch { namespace Concurrency {

class DestroyableWaiter : public Waiter
{
public:
    static DestroyableWaiter* Create(WaiterCallback callback, volatile void* data);

    void Destroy();

private:
    DestroyableWaiter(WaiterCallback callback, void* data);
};

inline DestroyableWaiter::DestroyableWaiter(WaiterCallback callback, void* data)
    : Waiter(callback, data)
{}

inline DestroyableWaiter* DestroyableWaiter::Create(WaiterCallback callback, volatile void* data)
{
    return new (Allocate()) DestroyableWaiter(callback, const_cast<void*>(data));
}

inline void DestroyableWaiter::Destroy()
{
    Free(this);
}

}}

#endif
