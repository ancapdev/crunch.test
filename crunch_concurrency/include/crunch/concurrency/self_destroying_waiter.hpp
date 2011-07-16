// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_CONCURRENCY_SELF_DESTROYING_WAITER_HPP
#define CRUNCH_CONCURRENCY_SELF_DESTROYING_WAITER_HPP

#include "crunch/concurrency/waiter.hpp"

namespace Crunch { namespace Concurrency {

class SelfDestroyingWaiter : public Waiter
{
public:
    template<typename F>
    static SelfDestroyingWaiter* Create(F callback);

private:
    template<typename F>
    struct FuncWrapper : Crunch::NonCopyable
    {
        FuncWrapper(void* a, F f) : allocation(a), func(f) {}

        void* allocation;
        F func;
    };

    SelfDestroyingWaiter() {}
    SelfDestroyingWaiter(WaiterCallback callback, void* data) : Waiter(callback, data) {}

    template<typename F>
    static void InvokeFunc(void* f);

    template<typename F>
    static void InvokeEmbeddedFunc(void* f);
};

template<typename F>
SelfDestroyingWaiter* SelfDestroyingWaiter::Create(F callback)
{
    if (sizeof(FuncWrapper<F>) > sizeof(StorageType))
    {
        FuncWrapper<F>* wrapper = new FuncWrapper<F>(Allocate(), callback);
        return new (wrapper->allocation) SelfDestroyingWaiter(&SelfDestroyingWaiter::InvokeFunc<F>, wrapper);
    }
    else
    {
        SelfDestroyingWaiter* node = reinterpret_cast<SelfDestroyingWaiter*>(Allocate());
        node->callback = &SelfDestroyingWaiter::InvokeEmbeddedFunc<F>;
        node->data = &node->storage;
        new (&node->storage) FuncWrapper<F>(node, callback);
        return node;
    }
}

template<typename F>
void SelfDestroyingWaiter::InvokeFunc(void* f)
{
    FuncWrapper<F>* wrapper = reinterpret_cast<FuncWrapper<F>*>(f);
    wrapper->func();
    Free(wrapper->allocation);
    delete wrapper;
}

template<typename F>
void SelfDestroyingWaiter::InvokeEmbeddedFunc(void* f)
{
    FuncWrapper<F>* wrapper = reinterpret_cast<FuncWrapper<F>*>(f);
    wrapper->func();
    wrapper->func.~F();
    Free(wrapper->allocation);
}

}}

#endif
