// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/base/assert.hpp"
#include "crunch/concurrency/mutex.hpp"
#include "crunch/concurrency/yield.hpp"

namespace Crunch { namespace Concurrency {

Mutex::Mutex()
    : mWaiters(reinterpret_cast<Waiter*>(MUTEX_FREE_BIT), MEMORY_ORDER_RELAXED)
{}

void Mutex::Release()
{
    ExponentialBackoff backoff;
    Waiter* head = mWaiters.Load(MEMORY_ORDER_RELAXED);
    for (;;)
    {
        CRUNCH_ASSERT_MSG((reinterpret_cast<std::size_t>(head) & MUTEX_FREE_BIT) == 0, "Attempting to release unlocked mutex");

        if (head == nullptr)
        {
            // No waiters, attempt to set free bit.
            if (mWaiters.CompareAndSwap(reinterpret_cast<Waiter*>(MUTEX_FREE_BIT), head))
                return;
        }
        else if (reinterpret_cast<std::size_t>(head) & LIST_LOCK_BIT)
        {
            // Wait for lock release.
            head = mWaiters.Load(MEMORY_ORDER_RELAXED);
        }
        else 
        {
            // Grab new head first to avoid write dependency on head being updated in CAS
            Waiter* newHead = head->next;
            CRUNCH_ASSERT((reinterpret_cast<std::size_t>(head) & MUTEX_FREE_BIT) == 0);

            // Lock list and signal first waiter
            if (mWaiters.CompareAndSwap(reinterpret_cast<Waiter*>(LIST_LOCK_BIT), head))
            {
                head->Notify();
                mWaiters.Store(newHead, MEMORY_ORDER_RELAXED);
                return;
            }
        }

        backoff.Pause();
    }
}

bool Mutex::IsLocked() const
{
    return reinterpret_cast<std::size_t>(mWaiters.Load(MEMORY_ORDER_RELAXED)) != MUTEX_FREE_BIT;
}

void Mutex::AddWaiter(Waiter* waiter)
{
    CRUNCH_ASSERT((reinterpret_cast<std::size_t>(waiter) & FLAG_BITS) == 0);

    ExponentialBackoff backoff;
    Waiter* head = mWaiters.Load(MEMORY_ORDER_RELAXED);
    for (;;)
    {
        if (reinterpret_cast<std::size_t>(head) & MUTEX_FREE_BIT)
        {
            // Mutex is unlocked. Attempt to lock.
            CRUNCH_ASSERT((reinterpret_cast<std::size_t>(head) & MUTEX_FREE_BIT) == MUTEX_FREE_BIT);
            if (mWaiters.CompareAndSwap(nullptr, head))
            {
                waiter->Notify();
                return;
            }
        }
        else if (reinterpret_cast<std::size_t>(head) & LIST_LOCK_BIT)
        {
            // List is locked. Reload head and retry.
            head = mWaiters.Load(MEMORY_ORDER_RELAXED);
        }
        else
        {
            // Mutex is locked, list is unlocked. Attempt to insert waiter
            waiter->next = head;
            if (mWaiters.CompareAndSwap(waiter, head))
                return;
        }

        backoff.Pause();
    }
}

bool Mutex::RemoveWaiter(Waiter* waiter)
{
    CRUNCH_ASSERT((reinterpret_cast<std::size_t>(waiter) & FLAG_BITS) == 0);

    ExponentialBackoff backoff;
    Waiter* head = mWaiters.Load(MEMORY_ORDER_RELAXED);

    for (;;)
    {
        if ((reinterpret_cast<std::size_t>(head) & LIST_LOCK_BIT) != 0)
        {
            head = mWaiters.Load(MEMORY_ORDER_RELAXED);
        }
        else if (mWaiters.CompareAndSwap(reinterpret_cast<Waiter*>(LIST_LOCK_BIT), head))
        {
            head = RemoveWaiterFromList(head, waiter);
            mWaiters.Store(head, MEMORY_ORDER_RELEASE);
            return true;
        }

        backoff.Pause();
    }
}

bool Mutex::IsOrderDependent() const
{
    return true;
}

}}
