// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/concurrency/waiter.hpp"
#include "crunch/concurrency/atomic.hpp"
#include "crunch/base/stdint.hpp"

#include <cstdlib>

namespace Crunch { namespace Concurrency {

namespace
{
#if (CRUNCH_PTR_SIZE == 4)
    uint64 const ABA_ADDEND = 4ull << 32;
    uint64 const PTR_MASK = 0xffffffffull;
#else
    // Assume 48 bit address space
    uint64 const ABA_ADDEND = 1ull << 48;
    uint64 const PTR_MASK = ABA_ADDEND - 1;
#endif

    Waiter* GetPointer(uint64 ptrAndState)
    {
        return reinterpret_cast<Waiter*>(ptrAndState & PTR_MASK);
    }

    uint64 SetPointer(uint64 ptrAndState, Waiter* ptr)
    {
        CRUNCH_ASSERT((reinterpret_cast<uint64>(ptr) & ~PTR_MASK) == 0);
        return (ptrAndState & ~PTR_MASK) | reinterpret_cast<uint64>(ptr);
    }

    Atomic<uint64> gWaiterFreeList;
}

CRUNCH_THREAD_LOCAL Waiter* Waiter::tWaiterFreeList = 0;

void* Waiter::AllocateGlobal()
{
    uint64 head = gWaiterFreeList.Load(MEMORY_ORDER_RELAXED);
    for (;;)
    {
        Waiter* headPtr = GetPointer(head);
        if (headPtr == nullptr)
        {
            return std::malloc(sizeof(Waiter));
        }
        else
        {
            uint64 const newHead = SetPointer(head, headPtr->next) + ABA_ADDEND;
            if (gWaiterFreeList.CompareAndSwap(newHead, head))
                return headPtr;
        }
    }
}

void Waiter::FreeGlobal(void* allocation)
{
    Waiter* node = reinterpret_cast<Waiter*>(allocation);
    uint64 head = gWaiterFreeList.Load(MEMORY_ORDER_RELAXED);
    for (;;)
    {
        node->next = GetPointer(head);
        uint64 const newHead = SetPointer(head, node) + ABA_ADDEND;
        if (gWaiterFreeList.CompareAndSwap(newHead, head))
            return;
    }
}

}}