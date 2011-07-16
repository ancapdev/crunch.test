// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_CONTAINERS_DETAIL_FIXED_VECTOR_BASE_HPP
#define CRUNCH_CONTAINERS_DETAIL_FIXED_VECTOR_BASE_HPP

#include "crunch/base/stdint.hpp"

#include <cstddef>
#include <type_traits>

namespace Crunch { namespace Containers { namespace Detail {

template<typename T, std::size_t S>
class FixedVectorBase
{
public:
    typedef T& reference;
    typedef T const& const_reference;
    typedef T* iterator;
    typedef T const* const_iterator;
    typedef uint32 size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T value_type;
    typedef T* pointer;
    typedef T const* const_pointer;

protected:
    typedef typename std::aligned_storage<sizeof(T) * S, std::alignment_of<T>::value>::type StorageType;

    T* GetStorage();
    T const* GetStorage() const;
    
    StorageType mStorage;
};

template<typename T, std::size_t S>
inline T* FixedVectorBase<T, S>::GetStorage()
{
    return reinterpret_cast<T*>(&mStorage);
}

template<typename T, std::size_t S>
inline T const* FixedVectorBase<T, S>::GetStorage() const
{
    return reinterpret_cast<T const*>(&mStorage);
}

}}}

#endif
