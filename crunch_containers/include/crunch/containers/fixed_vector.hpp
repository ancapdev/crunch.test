// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_CONTAINERS_FIXED_VECTOR_HPP
#define CRUNCH_CONTAINERS_FIXED_VECTOR_HPP

#include "crunch/containers/exception.hpp"
#include "crunch/containers/utility.hpp"
#include "crunch/containers/detail/fixed_vector_base.hpp"

#include <algorithm>
#include <iterator>
#include <memory>

namespace Crunch { namespace Containers {

// TODO: reverse iterators
// TODO: emplace operations
template<typename T, std::size_t S>
class FixedVector : public Detail::FixedVectorBase<T, S>
{
public:
    typedef Detail::FixedVectorBase<T, S> BaseType;
    typedef FixedVector<T, S> ThisType;

    //
    // Construction
    // 
    FixedVector();

    FixedVector(typename ThisType::size_type n, T const& value = T());

    template<typename InputIt>
    FixedVector(InputIt first, InputIt last);

    FixedVector(FixedVector<T, S> const& rhs);

    FixedVector(FixedVector<T, S>&& rhs);

    ~FixedVector();

    //
    // Assignment
    // 
    FixedVector& operator = (FixedVector<T, S> const& rhs);

    FixedVector& operator = (FixedVector<T, S>&& rhs);

    //
    // Iterators
    // 
    T* begin();
    T const* begin() const;
    T* end();
    T const* end() const;

    //
    // Capacity
    // 
    typename ThisType::size_type size() const;
    typename ThisType::size_type max_size() const;
    void resize(typename ThisType::size_type size);
    void resize(typename ThisType::size_type size, T value);
    typename ThisType::size_type capacity() const;
    bool empty() const;
    void reserve(typename ThisType::size_type size);

    //
    // Accessors
    // 
    T& operator [] (typename ThisType::size_type index);
    T const& operator [] (typename ThisType::size_type index) const;
    T& at(typename ThisType::size_type index);
    T const& at(typename ThisType::size_type index) const;
    T& front();
    T const& front() const;
    T& back();
    T const& back() const;

    //
    // Modifiers
    // 
    template<typename InputIt>
    void assign(InputIt first, InputIt last);
    void assign(typename ThisType::size_type n, T const& value);
    void push_back(T const& value);
    void push_back(T&& value);
    void pop_back();
    void insert(T* where, T const& value);
    // void insert(T* where, T&& value);
    void insert(T* where, typename ThisType::size_type n, T const& value);
    template<typename InputIt>
    void insert(T* where, InputIt first, InputIt last);
    T* erase(T* where);
    T* erase(T* first, T* last);
    void swap(FixedVector<T, S>& rhs);
    void clear();

private:
    // TODO: move utility functions to FixedVectorBase

    void move_assign(FixedVector<T, S>&& rhs);

    template<typename InputIt>
    void insert_impl(T* where, InputIt first, InputIt last, std::input_iterator_tag);

    template<typename InputIt>
    void insert_impl(T* where, InputIt first, InputIt last, std::forward_iterator_tag);

    typename ThisType::size_type mSize;
};


template<typename T, std::size_t S>
FixedVector<T, S>::FixedVector()
    : mSize(0)
{}

template<typename T, std::size_t S>
FixedVector<T, S>::FixedVector(typename ThisType::size_type n, T const& value)
    : mSize(0)
{
    assign(n, value);
}

template<typename T, std::size_t S>
template<typename InputIt>
FixedVector<T, S>::FixedVector(InputIt first, InputIt last)
    : mSize(0)
{
    assign(first, last);
}

template<typename T, std::size_t S>
FixedVector<T, S>::FixedVector(FixedVector<T, S> const& rhs)
    : mSize(0)
{
    assign(rhs.begin(), rhs.end());
}

template<typename T, std::size_t S>
FixedVector<T, S>::FixedVector(FixedVector<T, S>&& rhs)
    : mSize(0)
{
    move_assign(std::move(rhs));
}

template<typename T, std::size_t S>
FixedVector<T, S>::~FixedVector()
{
    for (std::size_t i = 0; i < mSize; ++i)
        this->GetStorage()[i].~T();
}

template<typename T, std::size_t S>
FixedVector<T, S>& FixedVector<T, S>::operator = (FixedVector<T, S> const& rhs)
{
    assign(rhs.begin(), rhs.end());
    return *this;
}

template<typename T, std::size_t S>
FixedVector<T, S>& FixedVector<T, S>::operator = (FixedVector<T, S>&& rhs)
{
    move_assign(std::move(rhs));
    return *this;
}

template<typename T, std::size_t S>
T* FixedVector<T, S>::begin()
{
    return this->GetStorage();
}

template<typename T, std::size_t S>
T const* FixedVector<T, S>::begin() const
{
    return this->GetStorage();
}

template<typename T, std::size_t S>
T* FixedVector<T, S>::end()
{
    return this->GetStorage() + mSize;
}

template<typename T, std::size_t S>
T const* FixedVector<T, S>::end() const
{
    return this->GetStorage() + mSize;
}

template<typename T, std::size_t S>
typename FixedVector<T, S>::size_type FixedVector<T, S>::size() const
{
    return mSize;
}

template<typename T, std::size_t S>
typename FixedVector<T, S>::size_type FixedVector<T, S>::max_size() const
{
    return S;
}

template<typename T, std::size_t S>
void FixedVector<T, S>::resize(typename ThisType::size_type size)
{
    if (size < mSize)
        erase(begin() + size, end());
    else if (mSize < size)
        insert(end(), T());
}

template<typename T, std::size_t S>
void FixedVector<T, S>::resize(typename ThisType::size_type size, T value)
{
    if (size < mSize)
        erase(begin() + size, end());
    else if (mSize < size)
        insert(end(), value);
}

template<typename T, std::size_t S>
typename FixedVector<T, S>::size_type FixedVector<T, S>::capacity() const
{
    return S;
}

template<typename T, std::size_t S>
bool FixedVector<T, S>::empty() const
{
    return mSize == 0;
}

template<typename T, std::size_t S>
void FixedVector<T, S>::reserve(typename ThisType::size_type size)
{
    if (size > max_size())
        ThrowLengthError("FixedVector::reserve() size over max size");
}

template<typename T, std::size_t S>
T& FixedVector<T, S>::operator [] (typename ThisType::size_type index)
{
    return this->GetStorage()[index];
}

template<typename T, std::size_t S>
T const& FixedVector<T, S>::operator [] (typename ThisType::size_type index) const
{
    return this->GetStorage()[index];
}

template<typename T, std::size_t S>
T& FixedVector<T, S>::at(typename ThisType::size_type index)
{
    if (index < mSize)
        return this->GetStorage()[index];
    else
        ThrowOutOfRange("FixedVector::at() out of range");
}

template<typename T, std::size_t S>
T const& FixedVector<T, S>::at(typename ThisType::size_type index) const
{
    if (index < mSize)
        return this->GetStorage()[index];
    else
        ThrowOutOfRange("FixedVector::at() out of range");
}

template<typename T, std::size_t S>
T& FixedVector<T, S>::front()
{
    return *this->GetStorage();
}

template<typename T, std::size_t S>
T const& FixedVector<T, S>::front() const
{
    return *this->GetStorage();
}

template<typename T, std::size_t S>
T& FixedVector<T, S>::back()
{
    return this->GetStorage()[mSize - 1];
}

template<typename T, std::size_t S>
T const& FixedVector<T, S>::back() const
{
    return this->GetStorage()[mSize - 1];
}

template<typename T, std::size_t S>
template<typename InputIt>
void FixedVector<T, S>::assign(InputIt first, InputIt last)
{
    erase(begin(), end());
    insert(begin(), first, last);
}

template<typename T, std::size_t S>
void FixedVector<T, S>::assign(typename ThisType::size_type n, T const& value)
{
    T const valueCopy = value;
    erase(begin(), end());
    insert(begin(), n, valueCopy);
}

template<typename T, std::size_t S>
void FixedVector<T, S>::move_assign(ThisType&& rhs)
{
    if (this == &rhs)
        return;

    erase(begin(), end());
    UninitializedMove(rhs.begin(), rhs.end(), begin());
    Destroy(rhs.begin(), rhs.end());
    mSize = rhs.mSize;
    rhs.mSize = 0;
}

template<typename T, std::size_t S>
void FixedVector<T, S>::push_back(T const& value)
{
    if (mSize >= S)
        ThrowLengthError("FixedVector::push_back grew too long");

    new (this->GetStorage() + mSize) T(value);
    mSize++;
}

template<typename T, std::size_t S>
void FixedVector<T, S>::push_back(T&& value)
{
    if (mSize >= S)
        ThrowLengthError("FixedVector::push_back grew too long");

    new (this->GetStorage() + mSize) T(std::forward<T>(value));
    mSize++;
}

template<typename T, std::size_t S>
void FixedVector<T, S>::pop_back()
{
    if (!empty())
        this->GetStorage()[--mSize].~T();
}

template<typename T, std::size_t S>
void FixedVector<T, S>::insert(T* where, T const& value)
{
    insert(where, 1, value);
}

template<typename T, std::size_t S>
void FixedVector<T, S>::insert(T* where, typename ThisType::size_type n, T const& value)
{
    if (n == 0)
        return;

    if (n + size() > capacity())
        ThrowLengthError("Fixedvector::insert grew too long");

    // In case value is in current range
    T const temp = value;

    if (where + n > end()) // spills off end
    {
        // Move to make space
        UninitializedMove(where, end(), where + n);

        try
        {
            // Fill over uninitialized end
            std::uninitialized_fill_n(end(), n - (end() - where), temp);
        }
        catch (...)
        {
            // Destroy previously moved items
            Destroy(where + n, end() + n);
            throw;
        }

        std::fill_n(where, end() - where, temp);
    }
    else // all overlapping current range
    {
        // Move last n elements
        UninitializedMove(end() - n, end(), end());
        // Copy remaing upwards
        std::copy_backward(where, end() - n, end());
        // Insert into hole
        std::fill_n(where, n, temp);
    }

    mSize += n;
}

template<typename T, std::size_t S>
template<typename InputIt>
void FixedVector<T, S>::insert(T* where, InputIt first, InputIt last)
{
    insert_impl(where, first, last, typename std::iterator_traits<InputIt>::iterator_category());
}

template<typename T, std::size_t S>
template<typename InputIt>
void FixedVector<T, S>::insert_impl(T* where, InputIt first, InputIt last, std::input_iterator_tag)
{
    if (first == last)
        return;

    typename ThisType::size_type const offset = where - begin();
    typename ThisType::size_type const oldSize = size();
    try
    {
        for (; first != last; ++first)
            push_back(*first);
    }
    catch (...)
    {
        erase(begin() + oldSize, end());
        throw;
    }

    std::rotate(begin() + offset, begin() + oldSize, end());
}

template<typename T, std::size_t S>
template<typename InputIt>
void FixedVector<T, S>::insert_impl(T* where, InputIt first, InputIt last, std::forward_iterator_tag)
{
    auto const count = static_cast<typename ThisType::size_type>(std::distance(first, last));
    if (count == 0)
        return;

    if (max_size() - size() < count)
        ThrowLengthError("FixedVector::insert grew too long");

    std::uninitialized_copy(first, last, end());
    std::rotate(where, end(), end() + count);
    mSize += count;
}

template<typename T, std::size_t S>
T* FixedVector<T, S>::erase(T* where)
{
    Move(where + 1, end(), where);
    mSize--;
    end()->~T();
    return where;
}

template<typename T, std::size_t S>
T* FixedVector<T, S>::erase(T* first, T* last)
{
    if (first == last)
        return first;

    T* newEnd = Move(last, end(), first);
    Destroy(newEnd, end());
    mSize = static_cast<typename ThisType::size_type>(newEnd - begin());
    return first;
}

template<typename T, std::size_t S>
void FixedVector<T, S>::swap(FixedVector<T, S>& rhs)
{
}

template<typename T, std::size_t S>
void FixedVector<T, S>::clear()
{
    Destroy(begin(), end());
    mSize = 0;
}

}}

#endif
