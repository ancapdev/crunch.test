#ifndef CRUNCH_CONCURRENCY_LOCK_GUARD_HPP
#define CRUNCH_CONCURRENCY_LOCK_GUARD_HPP

namespace Crunch { namespace Concurrency {

template<typename LockableType>
class LockGuard
{
public:
    LockGuard(LockableType& lockable)
        : mLockable(lockable)
    {
        mLockable.Lock();
    }

    ~LockGuard()
    {
        mLockable.Unlock();
    }

private:
    LockableType& mLockable;
};

}}

#endif
