#include "crunch/concurrency/event.hpp"

namespace Crunch { namespace Concurrency {

Event::Event(bool initialState)
    : mState(initialState)
    , mRootWaiter(NULL)
{ }

void Event::AddWaiter(Waiter* waiter)
{
    if (mState)
    {
        waiter->Wakeup();
    }
    else
    {
        boost::mutex::scoped_lock lock(mMutex);            
        waiter->next = mRootWaiter;
        mRootWaiter = waiter;
    }
}

void Event::RemoveWaiter(Waiter* waiter)
{
    boost::mutex::scoped_lock lock(mMutex);

    if (waiter == mRootWaiter)
    {
        mRootWaiter = mRootWaiter->next;
    }
    else
    {
        Waiter* currentWaiter = mRootWaiter;
        while (currentWaiter)
        {
            if (currentWaiter->next == waiter)
            {
                currentWaiter->next = currentWaiter->next->next;
                break;
            }
            currentWaiter = currentWaiter->next;
        }
    }
}

bool Event::IsOrderDependent() const
{
    return false;
}

void Event::Set()
{
    if (mState)
        return;

    boost::mutex::scoped_lock lock(mMutex);
    mState = true;
    Waiter* waiter = mRootWaiter;
    mRootWaiter = NULL;

    while (waiter)
    {
        Waiter* next = waiter->next;
        waiter->next = NULL;
        waiter->Wakeup();
        waiter = next;
    }
}

void Event::Clear()
{
    if (!mState)
        return;

    // Take lock so we don't flip state while signaling waiters
    boost::mutex::scoped_lock lock(mMutex);
    mState = false;
}

}}
