// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_CONTAINERS_FIXED_VECTOR_HPP
#define CRUNCH_CONTAINERS_FIXED_VECTOR_HPP

#include "crunch/containers/exception.hpp"
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
    //
    // Construction
    // 
    FixedVector();

    FixedVector(size_type n, T const& value = T());

    template<typename InputIt>
    FixedVector(InputIt first, InputIt last);

    template<std::size_t RhsSize>
    FixedVector(FixedVector<T, RhsSize> const& rhs);

    template<std::size_t RhsSize>
    FixedVector(FixedVector<T, RhsSize>&& rhs);

    ~FixedVector();

    //
    // Assignment
    // 
    template<std::size_t RhsSize>
    FixedVector& operator = (FixedVector<T, RhsSize> const& rhs);

    template<std::size_t RhsSize>
    FixedVector& operator = (FixedVector<T, RhsSize>&& rhs);

    //
    // Iterators
    // 
    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;

    //
    // Capacity
    // 
    size_type size() const;
    size_type max_size() const;
    void resize(size_type size);
    void resize(size_type size, T value);
    size_type capacity() const;
    bool empty() const;
    void reserve(size_type size);

    //
    // Accessors
    // 
    T& operator [] (size_type index);
    T const& operator [] (size_type index) const;
    T& at(size_type index);
    T const& at(size_type index) const;
    T& front();
    T const& front() const;
    T& back();
    T const& back() const;

    //
    // Modifiers
    // 
    template<typename InputIt>
    void assign(InputIt first, InputIt last);
    void assign(size_type n, T const& value = T());
    void push_back(T const& value);
    void push_back(T&& value);
    void pop_back();
    void insert(const_iterator where, T const& value);
    void insert(const_iterator where, T&& value);
    template<typename InputIt>
    void insert(const_iterator where, InputIt first, InputIt last);
    void erase(const_iterator where);
    void erase(const_iterator first, const_iterator last);
    void swap(FixedVector<T, S>& rhs);
    void clear();

private:
    // TODO: move utility functions to FixedVectorBase

    template<std::size_t RhsSize>
    void move_assign(FixedVector<T, RhsSize>&& rhs);

    static void move_down(iterator first, iterator last, iterator where);

    template<typename InputIt>
    void insert_impl(const_iterator where, InputIt first, InputIt last, std::input_iterator_tag);

    template<typename InputIt>
    void insert_impl(const_iterator where, InputIt first, InputIt last, std::forward_iterator_tag);

    size_type mSize;
};


template<typename T, std::size_t S>
FixedVector<T, S>::FixedVector()
    : mSize(0)
{}

template<typename T, std::size_t S>
FixedVector<T, S>::FixedVector(size_type n, T const& value)
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
template<std::size_t RhsSize>
FixedVector<T, S>::FixedVector(FixedVector<T, RhsSize> const& rhs)
    : mSize(0)
{
    assign(rhs.begin(), rhs.end());
}

template<typename T, std::size_t S>
template<std::size_t RhsSize>
FixedVector<T, S>::FixedVector(FixedVector<T, RhsSize>&& rhs)
    : mSize(rhs.mSize)
{
    move_assign(rhs);
}

template<typename T, std::size_t S>
FixedVector<T, S>::~FixedVector()
{
    for (std::size_t i = 0; i < mSize; ++i)
        GetStorage()[i].~T();
}

template<typename T, std::size_t S>
template<std::size_t RhsSize>
FixedVector<T, S>& FixedVector<T, S>::operator = (FixedVector<T, RhsSize> const& rhs)
{
    assign(rhs.begin(), rhs.end());
    return *this;
}

template<typename T, std::size_t S>
template<std::size_t RhsSize>
FixedVector<T, S>& FixedVector<T, S>::operator = (FixedVector<T, RhsSize>&& rhs)
{
    move_assign(rhs);
    return *this;
}

template<typename T, std::size_t S>
T* FixedVector<T, S>::begin()
{
    return GetStorage();
}

template<typename T, std::size_t S>
T const* FixedVector<T, S>::begin() const
{
    return GetStorage();
}

template<typename T, std::size_t S>
T* FixedVector<T, S>::end()
{
    return GetStorage() + mSize;
}

template<typename T, std::size_t S>
T const* FixedVector<T, S>::end() const
{
    return GetStorage() + mSize;
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
void FixedVector<T, S>::resize(size_type size)
{
    if (size < mSize)
        erase(begin() + size, end());
    else if (mSize < size)
        insert(end(), T());
}

template<typename T, std::size_t S>
void FixedVector<T, S>::resize(size_type size, T value)
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
void FixedVector<T, S>::reserve(size_type size)
{
    if (size > max_size())
        ThrowLengthError("FixedVector::reserve() size over max size");
}

template<typename T, std::size_t S>
T& FixedVector<T, S>::operator [] (size_type index)
{
    return GetStorage()[index];
}

template<typename T, std::size_t S>
T const& FixedVector<T, S>::operator [] (size_type index) const
{
    return GetStorage()[index];
}

template<typename T, std::size_t S>
T& FixedVector<T, S>::at(size_type index)
{
    if (index < mSize)
        return GetStorage()[index];
    else
        ThrowOutOfRange("FixedVector::at() out of range");
}

template<typename T, std::size_t S>
T const& FixedVector<T, S>::at(size_type index) const
{
    if (index < mSize)
        return GetStorage()[index];
    else
        ThrowOutOfRange("FixedVector::at() out of range");
}

template<typename T, std::size_t S>
T& FixedVector<T, S>::front()
{
    return *GetStorage();
}

template<typename T, std::size_t S>
T const& FixedVector<T, S>::front() const
{
    return *GetStorage();
}

template<typename T, std::size_t S>
T& FixedVector<T, S>::back()
{
    return GetStorage()[mSize - 1];
}

template<typename T, std::size_t S>
T const& FixedVector<T, S>::back() const
{
    return GetStorage()[index - 1];
}

template<typename T, std::size_t S>
template<typename InputIt>
void FixedVector<T, S>::assign(InputIt first, InputIt last)
{
    erase(begin(), end());
    insert(begin(), first, last);
}

template<typename T, std::size_t S>
void FixedVector<T, S>::assign(size_type n, T const& value)
{
    T const valueCopy = value;
    erase(begin(), end());
    insert(begin(), n, valueCopy);
}

template<typename T, std::size_t S>
void FixedVector<T, S>::push_back(T const& value)
{
    if (mSize >= S)
        ThrowLengthError("FixedVector::push_back grew too long");

    new (GetStorage() + mSize) T(value);
    mSize++;
}

template<typename T, std::size_t S>
void FixedVector<T, S>::push_back(T&& value)
{
    if (mSize >= S)
        ThrowLengthError("FixedVector::push_back grew too long");

    new (GetStorage() + mSize) T(std::forward<T>(value));
    mSize++;
}

template<typename T, std::size_t S>
void FixedVector<T, S>::pop_back()
{
    if (!empty())
        GetStorage()[mSize--].~T();
}

template<typename T, std::size_t S>
void FixedVector<T, S>::insert(const_iterator where, T const& value)
{
}

template<typename T, std::size_t S>
void FixedVector<T, S>::insert(const_iterator where, T&& value)
{
}

template<typename T, std::size_t S>
template<typename InputIt>
void FixedVector<T, S>::insert(const_iterator where, InputIt first, InputIt last)
{
    insert_impl(where, first, last, typename std::iterator_traits<InputIt>::iterator_category());
}

template<typename T, std::size_t S>
template<typename InputIt>
void FixedVector<T, S>::insert_impl(const_iterator where, InputIt first, InputIt last, std::input_iterator_tag)
{
    if (first == last)
        return;

    size_type const offset = where - begin();
    size_type const oldSize = size();
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
void FixedVector<T, S>::insert_impl(const_iterator where, InputIt first, InputIt last, std::forward_iterator_tag)
{
    auto const count = std::distance(first, last);
    if (count == 0)
        return;

    if (max_size() - size() < count)
        ThrowLengthError("FixedVector::insert grew too long");

    std::uninitialized_copy(first, last, begin());
    std::rotate(where, end(), end() + count);
    mSize += count;
}

template<typename T, std::size_t S>
void FixedVector<T, S>::erase(const_iterator where)
{
    move_down(where + 1, end(), where);
    mSize--;
    end()->~T();
    return where;
}

template<typename T, std::size_t S>
void FixedVector<T, S>::erase(const_iterator first, const_iterator last)
{
}

template<typename T, std::size_t S>
void FixedVector<T, S>::swap(FixedVector<T, S>& rhs)
{
}

template<typename T, std::size_t S>
void FixedVector<T, S>::clear()
{
}

template<typename T, std::size_t S>
void FixedVector<T, S>::move_down(iterator first, iterator last, iterator where)
{
    for (; first < last; ++first)
        *where++ = std::move(*first);
}

}}

#endif
