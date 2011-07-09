// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_CONCURRENCY_MUTEX_HPP
#define CRUNCH_CONCURRENCY_MUTEX_HPP

#include "crunch/base/override.hpp"
#include "crunch/concurrency/atomic.hpp"
#include "crunch/concurrency/waitable.hpp"

namespace Crunch { namespace Concurrency {

// Simple non-fair mutex. Will give lock to waiters in LIFO order.
class Mutex : public IWaitable
{
public:
    Mutex();

    void Release();

    bool IsLocked() const;

    virtual void AddWaiter(Waiter* waiter) CRUNCH_OVERRIDE;
    virtual bool RemoveWaiter(Waiter* waiter) CRUNCH_OVERRIDE;
    virtual bool IsOrderDependent() const CRUNCH_OVERRIDE;

private:
    static const std::size_t LIST_LOCK_BIT = 1;
    // Set when free rather than locked so we don't have to strip off
    // locked bit when building waiter list
    static const std::size_t MUTEX_FREE_BIT = 2;
    static const std::size_t FLAG_BITS = 3;

    // TODO: factor shared Event and Mutex waiter code into common class
    Atomic<Waiter*> mWaiters;
};

}}

#endif
