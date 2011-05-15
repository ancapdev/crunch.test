#ifndef CRUNCH_CONCURRENCY_LOCK_FREE_SLIST_HPP
#define CRUNCH_CONCURRENCY_LOCK_FREE_SLIST_HPP

#include "crunch/concurrency/atomic.hpp"

#include <cstdint>


namespace Crunch { namespace Concurrency {

template<typename T>
class LockFreeSList
{
public:
    LockFreeSList()
    {
        Root root = { nullptr, 0 };
        mRoot.Store(root);
    }
    
    __declspec(noinline) void Push(T* node)
    {
        union
        {
            Root value;
            typename Platform::AtomicWord<Root>::Type bits;
        } oldRoot;

        oldRoot.value = mRoot.Load(MEMORY_ORDER_RELAXED);

        for (;;)
        {
            node->next.Store(oldRoot.value.object, MEMORY_ORDER_RELAXED);
            
            Root const newRoot = { node, oldRoot.value.aba + 1 };

            if (mRoot.CompareAndSwap(newRoot, oldRoot.bits, MEMORY_ORDER_RELEASE))
                break;
        }
    }

    T* Pop()
    {

    }

private:
    struct Root
    {
        T* object;
        std::uintptr_t aba;
    };
    
    Atomic<Root> mRoot;
};

}}
#endif
