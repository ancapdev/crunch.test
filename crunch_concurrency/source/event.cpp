// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/concurrency/event.hpp"
#include "crunch/concurrency/yield.hpp"

namespace Crunch { namespace Concurrency {

Event::Event(bool initialState)
    : mWaiters(initialState ? reinterpret_cast<Waiter*>(STATE_BIT) : nullptr, MEMORY_ORDER_RELAXED)
{ }

void Event::AddWaiter(Waiter* waiter)
{
    CRUNCH_ASSERT((reinterpret_cast<std::size_t>(waiter) & FLAG_BITS) == 0);

    ExponentialBackoff backoff;

    Waiter* head = mWaiters.Load(MEMORY_ORDER_RELAXED);

    for (;;)
    {
        // Check for ready flag
        if (reinterpret_cast<std::size_t>(head) & STATE_BIT)
        {
            waiter->Notify();
            return;
        }

        // Check for lock flag
        if (reinterpret_cast<std::size_t>(head) & LOCK_BIT)
        {
            backoff.Pause();
            head = mWaiters.Load(MEMORY_ORDER_RELAXED);
        }
        else
        {
            // Attempt insertion
            waiter->next = head;
            if (mWaiters.CompareAndSwap(waiter, head))
                return;

            // New head is loaded as part of CAS
            backoff.Pause();
        }
    }
}

void Event::RemoveWaiter(Waiter* waiter)
{
    CRUNCH_ASSERT((reinterpret_cast<std::size_t>(waiter) & FLAG_BITS) == 0);

    ExponentialBackoff backoff;

    Waiter* head = mWaiters.Load(MEMORY_ORDER_RELAXED);

    for (;;)
    {
        if ((reinterpret_cast<std::size_t>(head) & LOCK_BIT) == 0 &&
            mWaiters.CompareAndSwap(reinterpret_cast<Waiter*>(LOCK_BIT), head))
        {
            Waiter* current = head;
            if (current == waiter)
            {
                head = current->next;
            }
            else
            {
                while (current->next != nullptr)
                {
                    if (current->next == waiter)
                    {
                        current->next = waiter->next;
                        break;
                    }
                 
                    current = current->next;
                }
            }

            mWaiters.Store(head, MEMORY_ORDER_RELEASE);

            return;
        }

        backoff.Pause();
    }
}

bool Event::IsOrderDependent() const
{
    return false;
}

void Event::Set()
{
    ExponentialBackoff backoff;

    Waiter* head = mWaiters.Load(MEMORY_ORDER_RELAXED);

    for (;;)
    {
        // If we are already set, return
        if ((reinterpret_cast<std::size_t>(head) & STATE_BIT) != 0)
            return;

        // Acquire lock and notify waiters
        // Set state bit on acquire so AddWaiter doesn't block on the lock
        if ((reinterpret_cast<std::size_t>(head) & LOCK_BIT) == 0 &&
            mWaiters.CompareAndSwap(reinterpret_cast<Waiter*>(LOCK_BIT | STATE_BIT), head))
        {
            Waiter* current = head;
            while (current != nullptr)
            {
                // Cache next now bacause we can't access waiter after calling Notify()
                Waiter* next = current->next;
                current->Notify();
                current = next;
            }

            // Release the lock, clear the list, and set state bit
            mWaiters.Store(reinterpret_cast<Waiter*>(STATE_BIT));

            return;
        }

        backoff.Pause();
    }
}

void Event::Reset()
{
    Waiter* head = mWaiters.Load(MEMORY_ORDER_RELAXED);
    
    ExponentialBackoff backoff;

    for (;;)
    {
        // If already reset, nothing to do
        if ((reinterpret_cast<std::size_t>(head) & STATE_BIT) == 0)
            return;

        // If the event is in a signalled stated it must either be in the process of signalling or empty
        CRUNCH_ASSERT((reinterpret_cast<std::size_t>(head) & ~LOCK_BIT) == STATE_BIT);

        // If lock is taken need to wait for it to complete
            if ((reinterpret_cast<std::size_t>(head) & LOCK_BIT) == 0 &&
            mWaiters.CompareAndSwap(nullptr, head))
            return;

        backoff.Pause();
    }
}

bool Event::IsSet() const
{
    return (reinterpret_cast<std::size_t>(mWaiters.Load(MEMORY_ORDER_RELAXED)) & STATE_BIT) != 0;
}

}}
