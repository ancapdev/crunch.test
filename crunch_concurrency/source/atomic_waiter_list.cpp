#include "crunch/concurrency/atomic_waiter_list.hpp"
#include "crunch/concurrency/yield.hpp"
#include "crunch/base/assert.hpp"

namespace Crunch { namespace Concurrency {

AtomicWaiterList::AtomicWaiterList()
    : mHead(nullptr)
{}

void AtomicWaiterList::AddWaiter(Waiter* waiter)
{
    CRUNCH_ASSERT((reinterpret_cast<std::size_t>(waiter) & FLAG_BITS) == 0);

    ExponentialBackoff backoff;

    Waiter* head = mHead.Load(MEMORY_ORDER_RELAXED);

    for (;;)
    {
        // Check for ready flag
        if (reinterpret_cast<std::size_t>(head) & READY_BIT)
        {
            waiter->Wakeup();
            return;
        }

        // Check for lock flag
        if (reinterpret_cast<std::size_t>(head) & LOCK_BIT)
        {
            backoff.Pause();
            head = mHead.Load(MEMORY_ORDER_RELAXED);
        }
        else
        {
            // Attempt insertion
            waiter->next = head;
            if (mHead.CompareAndSwap(waiter, head))
                return;

            // New head is loaded as part of CAS
            backoff.Pause();
        }
    }
}

void AtomicWaiterList::RemoveWaiter(Waiter* waiter)
{
    CRUNCH_ASSERT((reinterpret_cast<std::size_t>(waiter) & FLAG_BITS) == 0);

    ExponentialBackoff backoff;

    Waiter* head = mHead.Load(MEMORY_ORDER_RELAXED);

    for (;;)
    {
        if ((reinterpret_cast<std::size_t>(head) & LOCK_BIT) == 0 &&
            mHead.CompareAndSwap(reinterpret_cast<Waiter*>(LOCK_BIT), head))
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

            mHead.Store(head, MEMORY_ORDER_RELEASE);

            return;
        }

        backoff.Pause();
    }
}

void AtomicWaiterList::NotifyWaiters()
{
    ExponentialBackoff backoff;

    Waiter* head = mHead.Load(MEMORY_ORDER_RELAXED);

    for (;;)
    {
        if ((reinterpret_cast<std::size_t>(head) & LOCK_BIT) == 0 &&
            mHead.CompareAndSwap(reinterpret_cast<Waiter*>(LOCK_BIT | READY_BIT), head)) // Set ready bit early to allow AddWaiter to exit without waiting for lock
        {
            Waiter* current = head;
            while (current != nullptr)
            {
                // Cache next now bacause we can't access waiter after calling Wakeup()
                Waiter* next = current->next;
                current->Wakeup();
                current = next;
            }

            // Release the lock, clear the list, and set the ready bit
            mHead.Store(reinterpret_cast<Waiter*>(READY_BIT));

            return;
        }

        backoff.Pause();
    }
}

}}
