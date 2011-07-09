// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_CONCURRENCY_WAITABLE_HPP
#define CRUNCH_CONCURRENCY_WAITABLE_HPP

#include "crunch/base/override.hpp"
#include "crunch/base/stdint.hpp"

#include <utility>
#include <vector>

namespace Crunch { namespace Concurrency {

struct Waiter
{
    virtual void Notify() = 0;

    Waiter* next;
};

/// Remove waiter from list
/// \param head Start of list
/// \param waiter The waiter to remove
/// \return New head
inline Waiter* RemoveWaiterFromList(Waiter* head, Waiter* waiter)
{
    if (head == waiter)
        return head->next;

    Waiter* current = head;
    while (current->next != nullptr)
    {
        if (current->next == waiter)
        {
            current->next = waiter->next;
            break;
        }

        current = current->next;
    }
    return head;
}

template<typename F>
struct GenericWaiter : Waiter
{
    GenericWaiter(F const& f) : f(f) {}
    GenericWaiter(F&& f) : f(std::move(f)) {}
    GenericWaiter(GenericWaiter const& rhs) : f(rhs.f) {}
    GenericWaiter& operator = (GenericWaiter const& rhs) { f = rhs.f; return *this; }

    virtual void Notify() CRUNCH_OVERRIDE
    {
        f();
    }

    F f;
};

template<typename F>
GenericWaiter<F> MakeWaiter(F const& f)
{
    return GenericWaiter<F>(f);
}

template<typename F>
GenericWaiter<F> MakeWaiter(F&& f)
{
    return GenericWaiter<F>(std::move(f));
}

struct IWaitable
{
    virtual void AddWaiter(Waiter* waiter) = 0;

    virtual bool RemoveWaiter(Waiter* waiter) = 0;

    virtual bool IsOrderDependent() const = 0;

    virtual ~IWaitable() { }
};

struct WaitMode
{
    WaitMode(uint32 spinCount, bool runCooperative)
        : spinCount(spinCount)
        , runCooperative(runCooperative)
    {}

    static WaitMode Poll()
    {
        return WaitMode(0xfffffffful, false);
    }

    static WaitMode Block(uint32 spinCount = 0)
    {
        return WaitMode(spinCount, false);
    }

    static WaitMode Run(uint32 spinCount = 0)
    {
        return WaitMode(spinCount, true);
    }

    uint32 spinCount;
    bool runCooperative;
};

void WaitFor(IWaitable& waitable, WaitMode waitMode = WaitMode::Run());
void WaitForAll(IWaitable** waitables, std::size_t count, WaitMode waitMode = WaitMode::Run());

///\ return List of signaled waitables
std::vector<IWaitable*> WaitForAny(IWaitable** waitables, std::size_t count, WaitMode waitMode = WaitMode::Run());

}}

#endif
