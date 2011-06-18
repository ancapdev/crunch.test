#ifndef CRUNCH_CONCURRENCY_DETAIL_FUTURE_DATA_HPP
#define CRUNCH_CONCURRENCY_DETAIL_FUTURE_DATA_HPP

#include "crunch/concurrency/event.hpp"

#include <type_traits>
#include <utility>
#include <cstddef>

namespace Crunch { namespace Concurrency { namespace Detail {

template<typename T>
class FutureData
{
public:
    void Set(T const& value)
    {
        ::new (ResultAddress()) T(value);
        mSetEvent.Set();
    }

    void Set(T&& value)
    {
        ::new (ResultAddress()) T(std::move(value));
        mSetEvent.Set();
    }
    

private:
    typedef std::aligned_storage<sizeof(T), std::alignment_of<T>::value> StorageType;

    void* ResultAddress() { return reinterpret_cast<void*>(&mResult); }

    Event mSetEvent;
    StorageType mResult;
};

}}}

#endif
