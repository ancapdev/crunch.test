#ifndef CRUNCH_CONCURRENCY_WAITABLE_HPP
#define CRUNCH_CONCURRENCY_WAITABLE_HPP

#include "crunch/base/override.hpp"

#include <utility>

namespace Crunch { namespace Concurrency {

struct Waiter
{
    virtual void Notify() = 0;

    Waiter* next;
};

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

    // Must not return until any callbacks on waiter has completed
    virtual void RemoveWaiter(Waiter* waiter) = 0;

    virtual bool IsOrderDependent() const = 0;

    virtual ~IWaitable() { }
};


enum WaitMode
{
    WAIT_MODE_POLL,
    WAIT_MODE_YIELD_PREEMTIVE,
    WAIT_MODE_YIELD_COOPERATIVE
};

void WaitFor(IWaitable& waitable, WaitMode waitMode = WAIT_MODE_YIELD_COOPERATIVE);
void WaitForAll(IWaitable** waitables, std::size_t count, WaitMode waitMode = WAIT_MODE_YIELD_COOPERATIVE);
void WaitForAny(IWaitable** waitables, std::size_t count, WaitMode waitMode = WAIT_MODE_YIELD_COOPERATIVE);

}}

#endif
