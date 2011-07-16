// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_CONCURRENCY_WAITER_HPP
#define CRUNCH_CONCURRENCY_WAITER_HPP

#include "crunch/base/assert.hpp"
#include "crunch/base/noncopyable.hpp"
#include "crunch/concurrency/thread_local.hpp"

#include <memory>
#include <type_traits>

namespace Crunch { namespace Concurrency {

typedef void (*WaiterCallback)(void*);

class Waiter : NonCopyable
{
public:
    void Notify() const;

    Waiter* next;

protected:
    Waiter() {}
    Waiter(WaiterCallback callback, void* data);

    static void* Allocate();
    static void* AllocateGlobal();
    static void Free(void* allocation);
    static void FreeGlobal(void* allocation);

    typedef std::aligned_storage<16, 8>::type StorageType;

    WaiterCallback callback;
    void* data;
    StorageType storage;

    static CRUNCH_THREAD_LOCAL Waiter* tWaiterFreeList;
};

inline void Waiter::Notify() const
{
    callback(data);
}

inline Waiter::Waiter(WaiterCallback callback, void* data)
    : callback(callback)
    , data(data)
{}

inline void* Waiter::Allocate()
{
    Waiter* localFree = tWaiterFreeList;
    if (localFree)
    {
        tWaiterFreeList = localFree->next;
        return localFree;
    }
    return AllocateGlobal();
}

inline void Waiter::Free(void* allocation)
{
    // TODO: free back to global list
    Waiter* node = reinterpret_cast<Waiter*>(allocation);
    node->next = tWaiterFreeList;
    tWaiterFreeList = node;
}

}}

#endif
