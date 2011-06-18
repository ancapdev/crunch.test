#ifndef CRUNCH_CONCURRENCY_ATOMIC_WAITER_LIST_HPP
#define CRUNCH_CONCURRENCY_ATOMIC_WAITER_LIST_HPP

#include "crunch/concurrency/atomic.hpp"
#include "crunch/concurrency/waitable.hpp"

namespace Crunch { namespace Concurrency {

class AtomicWaiterList
{
public:
    AtomicWaiterList();

    void AddWaiter(Waiter* waiter);

    void RemoveWaiter(Waiter* waiter);

    void NotifyWaiters();

private:
    static const std::size_t LOCK_BIT = 1;
    static const std::size_t READY_BIT = 2;
    static const std::size_t FLAG_BITS = 3;

    // Reserves lower 2 bits of pointer. I.e., pointer must be 4 byte aligned
    // Bit 0 = lock bit
    // Bit 1 = ready bit
    // Doesn't need to ABA protection because removal is always by lock
    Atomic<Waiter*> mHead;
};

}}

#endif
