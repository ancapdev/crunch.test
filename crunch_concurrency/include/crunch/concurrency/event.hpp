#include "crunch/base/override.hpp"
#include "crunch/concurrency/atomic.hpp"
#include "crunch/concurrency/waitable.hpp"

namespace Crunch { namespace Concurrency {

// TODO: Make AddWaiter run concurrently with RemoveWaiter by having RemoveWaiter
//       reset the list on lock and and concatenate the old and new list on unlock.
//       Complexity O(N) in the number of waiters in the original list
//       Already the same for actual removal
class Event : public IWaitable
{
public:
    Event(bool initialState);

    void Set();
    void Reset();
    bool IsSet() const;

    virtual void AddWaiter(Waiter* waiter) CRUNCH_OVERRIDE;
    virtual void RemoveWaiter(Waiter* waiter) CRUNCH_OVERRIDE;
    virtual bool IsOrderDependent() const CRUNCH_OVERRIDE;

private:
    static const std::size_t LOCK_BIT = 1;
    static const std::size_t STATE_BIT = 2;
    static const std::size_t FLAG_BITS = 3;

    // Reserves lower 2 bits of pointer. I.e., pointer must be 4 byte aligned
    // Bit 0 = lock bit
    // Bit 1 = state bit
    // Doesn't need to ABA protection because removal is always by lock
    Atomic<Waiter*> mWaiters;
};

}}
