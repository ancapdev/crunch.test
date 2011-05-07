#include "crunch/base/override.hpp"
#include "crunch/concurrency/waitable.hpp"

#include <boost/thread/mutex.hpp>

namespace Crunch { namespace Concurrency {

class Event : public IWaitable
{
public:
    Event(bool initialState);

    void Set();
    void Clear();
    bool IsSet() const { return mState; }

    virtual void AddWaiter(Waiter* waiter) CRUNCH_OVERRIDE;
    virtual void RemoveWaiter(Waiter* waiter) CRUNCH_OVERRIDE;
    virtual bool IsOrderDependent() const CRUNCH_OVERRIDE;

private:
    volatile bool mState;
    boost::mutex mMutex;
    Waiter* mRootWaiter;
};

}}
