#ifndef CRUNCH_CONCURRENCY_WAITABLE_HPP
#define CRUNCH_CONCURRENCY_WAITABLE_HPP

namespace Crunch { namespace Concurrency {

struct Waiter
{
    virtual void Notify() = 0;

    Waiter* next;
};

struct IWaitable
{
    virtual void AddWaiter(Waiter* waiter) = 0;

    // Must not return until any callbacks on waiter has completed
    virtual void RemoveWaiter(Waiter* waiter) = 0;

    virtual bool IsOrderDependent() const = 0;

    virtual ~IWaitable() { }
};

}}

#endif
