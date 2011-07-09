// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/concurrency/event.hpp"
#include "crunch/concurrency/yield.hpp"

namespace Crunch { namespace Concurrency {

Event::Event(bool initialState)
    : mList(initialState ? EVENT_SET_BIT : 0, MEMORY_ORDER_RELAXED)
{}

void Event::AddWaiter(Waiter* waiter)
{
    ExponentialBackoff backoff;
    uint64 head = mList.Load(MEMORY_ORDER_RELAXED);
    for (;;)
    {
        if (head & EVENT_SET_BIT)
        {
            waiter->Notify();
            return;
        }

        waiter->next = GetPointer(head);
        uint64 const newHead = SetPointer(head, waiter) + ABA_ADDEND;
        if (mList.CompareAndSwap(newHead, head))
            return;

        backoff.Pause();
    }
}

bool Event::RemoveWaiter(Waiter* waiter)
{
    ExponentialBackoff backoff;
    uint64 head = mList.Load(MEMORY_ORDER_RELAXED);
    for (;;)
    {
        // If another remove is in progress, wait for completion
        if (head & LOCK_BIT)
        {
            head = mList.Load(MEMORY_ORDER_RELAXED);
        }
        else
        {
            // If empty and not locked, nothing to remove
            if ((head & (PTR_MASK | LOCK_BIT)) == 0)
                return false;

            Waiter* const headPtr = GetPointer(head);
            if (headPtr == waiter)
            {
                // If waiter is at head, do simple pop
                uint64 const newHead = SetPointer(head, headPtr->next) + ABA_ADDEND;
                if (mList.CompareAndSwap(newHead, head))
                    return true;
            }
            else
            {
                // Need to take ownership of list. Lock and swap with empty list.
                uint64 const empty = ((head & ~PTR_MASK) | LOCK_BIT) + ABA_ADDEND;
                if (mList.CompareAndSwap(empty, head))
                {
                    bool removed = false;
                    Waiter* current = headPtr;
                    while (current->next != nullptr)
                    {
                        if (current->next == waiter)
                        {
                            current->next = waiter->next;
                            removed = true;
                            break;
                        }

                        current = current->next;
                    }
                    Waiter* tail = current;
                    while (tail->next != nullptr)
                        tail = tail->next;

                    // Add list back
                    uint64 currentHead = empty;
                    for (;;)
                    {
                        CRUNCH_ASSERT((currentHead & EVENT_SET_BIT) == 0);
                        CRUNCH_ASSERT((currentHead & LOCK_BIT) != 0);

                        tail->next = GetPointer(currentHead);
                        uint64 const newHead = (SetPointer(currentHead, headPtr) ^ LOCK_BIT) + ABA_ADDEND;
                        if (mList.CompareAndSwap(newHead, currentHead))
                            return true;

                        backoff.Pause();
                    }
                }
            }
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
    uint64 head = mList.Load(MEMORY_ORDER_RELAXED);
    for (;;)
    {
        // If already set, return
        if (head & EVENT_SET_BIT)
            return;

        // Wait for list to be unlocked before notifying waiters
        if ((head & LOCK_BIT) == 0)
        {
            if ((head & PTR_MASK) == 0)
            {
                // No waiters to notify, only need to set state bit
                uint64 const newHead = (head | EVENT_SET_BIT) + ABA_ADDEND;
                if (mList.CompareAndSwap(newHead, head))
                    return;
            }
            else
            {
                // Need to acquire lock on list and notify waiters
                uint64 const lockedHead = ((head & ~PTR_MASK) | LOCK_BIT | EVENT_SET_BIT) + ABA_ADDEND;
                if (mList.CompareAndSwap(lockedHead, head))
                {
                    NotifyAllWaiters(GetPointer(head));
                    // Clear lock flag. Event may be have been reset so xor in rather than store the unlock
                    mList.Xor(LOCK_BIT, MEMORY_ORDER_RELEASE);
                    return;
                }
            }
        }

        backoff.Pause();
    }
}

void Event::Reset()
{
    mList.And(~EVENT_SET_BIT);
}

bool Event::IsSet() const
{
    return (mList.Load(MEMORY_ORDER_RELAXED) & EVENT_SET_BIT) != 0;
}

}}
