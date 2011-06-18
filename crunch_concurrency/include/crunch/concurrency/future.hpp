#ifndef CRUNCH_CONCURRENCY_FUTURE_HPP
#define CRUNCH_CONCURRENCY_FUTURE_HPP

#include "crunch/base/override.hpp"
#include "crunch/concurrency/waitable.hpp"
#include "crunch/concurrency/detail/future_data.hpp"

#include <memory>

namespace Crunch { namespace Concurrency {

template<typename T>
class Future : public IWaitable
{
public:
    //
    // IWaitable
    //
    virtual void AddWaiter(Waiter* waiter) CRUNCH_OVERRIDE;
    virtual void RemoveWaiter(Waiter* waiter) CRUNCH_OVERRIDE;
    virtual bool IsOrderDependent() const CRUNCH_OVERRIDE;

private:
    typedef Detail::FutureData<T> DataType;
    typedef std::shared_ptr<DataType> DataPtr;

    DataPtr mData;
};

template<>
class Future<void> : public IWaitable
{
public:
    //
    // IWaitable
    //
    virtual void AddWaiter(Waiter* waiter) CRUNCH_OVERRIDE;
    virtual void RemoveWaiter(Waiter* waiter) CRUNCH_OVERRIDE;
    virtual bool IsOrderDependent() const CRUNCH_OVERRIDE;

private:

};

}}

#endif
