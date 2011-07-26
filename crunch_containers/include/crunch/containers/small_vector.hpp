// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_CONTAINERS_SMALL_VECTOR_HPP
#define CRUNCH_CONTAINERS_SMALL_VECTOR_HPP

#include "crunch/containers/detail/fixed_vector_base.hpp"
#include "crunch/containers/exception.hpp"
#include "crunch/containers/utility.hpp"

#include <algorithm>
#include <memory>

namespace Crunch { namespace Containers {

template<typename T, std::size_t S, typename A = std::allocator<T>>
class SmallVector : public Detail::FixedVectorBase<T, S>
{
public:
    typedef typename A::template rebind<T>::other AllocatorType;
    typedef SmallVector<T, S, A> ThisType;
    typedef typename AllocatorType::reference reference;
    typedef typename AllocatorType::const_reference const_reference;
    typedef T* iterator;
    typedef T const* const_iterator;
    typedef typename AllocatorType::size_type size_type;
    typedef typename AllocatorType::difference_type difference_type;
    typedef typename AllocatorType::value_type value_type;
    typedef typename AllocatorType::pointer pointer;
    typedef typename AllocatorType::const_pointer const_pointer;
    
    //
    // Construction
    // 
    SmallVector(AllocatorType const& allocator = AllocatorType());

    SmallVector(size_type n, T const& value = T(), AllocatorType const& allocator = AllocatorType());

    template<typename InputIt>
    SmallVector(InputIt first, InputIt last, AllocatorType const& allocator = AllocatorType());

    SmallVector(SmallVector<T, S, A> const& rhs);

    SmallVector(SmallVector<T, S, A>&& rhs);

    ~SmallVector();

    //
    // Assignment
    // 
    SmallVector& operator = (SmallVector<T, S, A> const& rhs);

    SmallVector& operator = (SmallVector<T, S, A>&& rhs);

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
    size_type size() const;
    size_type max_size() const;
    void resize(size_type size);
    void resize(size_type size, T value);
    size_type capacity() const;
    bool empty() const;
    void reserve(size_type count);

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
    void assign(size_type n, T const& value);
    void push_back(T const& value);
    void push_back(T&& value);
    void pop_back();
    void insert(T* where, T const& value);
    // void insert(T* where, T&& value);
    void insert(T* where, size_type n, T const& value);
    template<typename InputIt>
    void insert(T* where, InputIt first, InputIt last);
    T* erase(T* where);
    T* erase(T* first, T* last);
    void swap(SmallVector<T, S, A>& rhs);
    void clear();

private:
    size_type new_capacity(size_type requiredSize) const;
    void reserve_more(size_type n);
    void move_assign(ThisType&& rhs);

    template<typename InputIt>
    void insert_impl(T* where, InputIt first, InputIt last, std::input_iterator_tag);

    template<typename InputIt>
    void insert_impl(T* where, InputIt first, InputIt last, std::forward_iterator_tag);

    T* mFirst;
    T* mLast;
    T* mEnd;
    AllocatorType mAllocator;
};

template<typename T, std::size_t S, typename A>
SmallVector<T, S, A>::SmallVector(AllocatorType const& allocator)
    : mFirst(this->GetStorage())
    , mLast(mFirst)
    , mEnd(mFirst + S)
    , mAllocator(allocator)
{}

template<typename T, std::size_t S, typename A>
SmallVector<T, S, A>::SmallVector(size_type n, T const& value, AllocatorType const& allocator)
    : mFirst(this->GetStorage())
    , mLast(mFirst)
    , mEnd(mFirst + S)
    , mAllocator(allocator)
{
    assign(n, value);
}

template<typename T, std::size_t S, typename A>
template<typename InputIt>
SmallVector<T, S, A>::SmallVector(InputIt first, InputIt last, AllocatorType const& allocator)
    : mFirst(this->GetStorage())
    , mLast(mFirst)
    , mEnd(mFirst + S)
    , mAllocator(allocator)
{
    assign(first, last);
}

template<typename T, std::size_t S, typename A>
SmallVector<T, S, A>::SmallVector(SmallVector<T, S, A> const& rhs)
    : mFirst(this->GetStorage())
    , mLast(mFirst)
    , mEnd(mFirst + S)
    , mAllocator(rhs.mAllocator)
{
    assign(rhs.begin(), rhs.end());
}

template<typename T, std::size_t S, typename A>
SmallVector<T, S, A>::SmallVector(SmallVector<T, S, A>&& rhs)
    : mFirst(this->GetStorage())
    , mLast(mFirst)
    , mEnd(mFirst + S)
    , mAllocator(rhs.mAllocator)
{
    move_assign(std::move(rhs));
}

template<typename T, std::size_t S, typename A>
SmallVector<T, S, A>::~SmallVector()
{
    Destroy(begin(), end(), mAllocator);
    if (mFirst != this->GetStorage())
        mAllocator.deallocate(mFirst, capacity());
}

template<typename T, std::size_t S, typename A>
SmallVector<T, S, A>& SmallVector<T, S, A>::operator = (SmallVector<T, S, A> const& rhs)
{
    assign(rhs.begin(), rhs.end());
    return *this;
}

template<typename T, std::size_t S, typename A>
SmallVector<T, S, A>& SmallVector<T, S, A>::operator = (SmallVector<T, S, A>&& rhs)
{
    move_assign(std::move(rhs));
    return *this;
}

template<typename T, std::size_t S, typename A>
T* SmallVector<T, S, A>::begin()
{
    return mFirst;
}

template<typename T, std::size_t S, typename A>
T const* SmallVector<T, S, A>::begin() const
{
    return mFirst;
}

template<typename T, std::size_t S, typename A>
T* SmallVector<T, S, A>::end()
{
    return mLast;
}

template<typename T, std::size_t S, typename A>
T const* SmallVector<T, S, A>::end() const
{
    return mLast;
}

template<typename T, std::size_t S, typename A>
typename SmallVector<T, S, A>::size_type SmallVector<T, S, A>::size() const
{
    return mLast - mFirst;
}

template<typename T, std::size_t S, typename A>
typename SmallVector<T, S, A>::size_type SmallVector<T, S, A>::max_size() const
{
    return mAllocator.max_size();
}

template<typename T, std::size_t S, typename A>
void SmallVector<T, S, A>::resize(size_type size)
{
    if (size < size())
        erase(begin() + size, end());
    else if (size() < size)
        insert(end(), T());
}

template<typename T, std::size_t S, typename A>
void SmallVector<T, S, A>::resize(size_type size, T value)
{
    if (size < size())
        erase(begin() + size, end());
    else if (size() < size)
        insert(end(), value);
}

template<typename T, std::size_t S, typename A>
typename SmallVector<T, S, A>::size_type SmallVector<T, S, A>::capacity() const
{
    return mEnd - mFirst;
}

template<typename T, std::size_t S, typename A>
bool SmallVector<T, S, A>::empty() const
{
    return mFirst == mLast;
}

template<typename T, std::size_t S, typename A>
void SmallVector<T, S, A>::reserve(size_type count)
{
    if (count > max_size())
        ThrowLengthError("SmallVector::reserve() size over max size");

    if (count > capacity())
    {
        pointer p = mAllocator.allocate(count);

        try
        {
            UninitializedMove(mFirst, mLast, p);
        }
        catch (...)
        {
            mAllocator.deallocate(p, count);
            throw;
        }

        Destroy(begin(), end(), mAllocator);
        if (mFirst != this->GetStorage())
            mAllocator.deallocate(mFirst, capacity());
        size_type s = size();
        mFirst = p;
        mLast = p + s;
        mEnd = p + count;
    }
}

template<typename T, std::size_t S, typename A>
typename SmallVector<T, S, A>::size_type SmallVector<T, S, A>::new_capacity(size_type requiredSize) const
{
    size_type const c = capacity();
    if (max_size() - c < c)
        return requiredSize;
    else
        return std::max(c * 2, requiredSize);
}

template<typename T, std::size_t S, typename A>
T& SmallVector<T, S, A>::operator [] (size_type index)
{
    return mFirst[index];
}

template<typename T, std::size_t S, typename A>
T const& SmallVector<T, S, A>::operator [] (size_type index) const
{
    return mFirst[index];
}

template<typename T, std::size_t S, typename A>
T& SmallVector<T, S, A>::at(size_type index)
{
    if (index < size())
        return mFirst[index];
    else
        ThrowOutOfRange("SmallVector::at() out of range");
}

template<typename T, std::size_t S, typename A>
T const& SmallVector<T, S, A>::at(size_type index) const
{
    if (index < size())
        return mFirst[index];
    else
        ThrowOutOfRange("SmallVector::at() out of range");
}

template<typename T, std::size_t S, typename A>
T& SmallVector<T, S, A>::front()
{
    return *mFirst;
}

template<typename T, std::size_t S, typename A>
T const& SmallVector<T, S, A>::front() const
{
    return *mFirst;
}

template<typename T, std::size_t S, typename A>
T& SmallVector<T, S, A>::back()
{
    return *(mLast - 1);
}

template<typename T, std::size_t S, typename A>
T const& SmallVector<T, S, A>::back() const
{
    return *(mLast - 1);
}

template<typename T, std::size_t S, typename A>
template<typename InputIt>
void SmallVector<T, S, A>::assign(InputIt first, InputIt last)
{
    erase(begin(), end());
    insert(begin(), first, last);
}

template<typename T, std::size_t S, typename A>
void SmallVector<T, S, A>::assign(size_type n, T const& value)
{
    T const valueCopy = value;
    erase(begin(), end());
    insert(begin(), n, valueCopy);
}

template<typename T, std::size_t S, typename A>
void SmallVector<T, S, A>::move_assign(ThisType&& rhs)
{
    if (this == &rhs)
        return;

    clear();
    if (mFirst != this->GetStorage())
        mAllocator.deallocate(mFirst, capacity());

    if (mAllocator == rhs.mAllocator &&
        rhs.mFirst != rhs.GetStorage())
    {
        // Steal allocated storage
        mFirst = rhs.mFirst;
        mLast = rhs.mLast;
        mEnd = rhs.mEnd;
        rhs.mFirst = rhs.GetStorage();
        rhs.mLast = rhs.mFirst;
        rhs.mEnd = rhs.mFirst + S;
    }
    else
    {
        // Copy embedded storage
        size_type const rhsSize = rhs.size();
        reserve(rhsSize);
        UninitializedMove(rhs.begin(), rhs.end(), mFirst);
        Destroy(rhs.begin(), rhs.end(), rhs.mAllocator);
        mLast = mFirst + rhsSize;
        rhs.mLast = rhs.mFirst;
    }
}


template<typename T, std::size_t S, typename A>
void SmallVector<T, S, A>::push_back(T const& value)
{
    if (mLast != mEnd)
    {
        mAllocator.construct(mLast++, value);
    }
    else
    {
        T const valueCopy = value;
        reserve_more(1);
        mAllocator.construct(mLast++, valueCopy);
    }
}

template<typename T, std::size_t S, typename A>
void SmallVector<T, S, A>::push_back(T&& value)
{
    if (mLast != mEnd)
    {
        mAllocator.construct(mLast++, std::forward<T>(value));
    }
    else
    {
        T valueCopy = std::forward<T>(value);
        reserve_more(1);
        mAllocator.construct(mLast++, std::move(valueCopy));
    }
}

template<typename T, std::size_t S, typename A>
void SmallVector<T, S, A>::reserve_more(size_type n)
{
    size_type const s = size();
    if (max_size() - n < s)
        ThrowLengthError("SmallVector::push_back grew too large");

    reserve(new_capacity(s + n));
}

template<typename T, std::size_t S, typename A>
void SmallVector<T, S, A>::pop_back()
{
    if (!empty())
        mAllocator.destroy(--mLast);
}

template<typename T, std::size_t S, typename A>
void SmallVector<T, S, A>::insert(T* where, T const& value)
{
    insert(where, 1, value);
}

template<typename T, std::size_t S, typename A>
void SmallVector<T, S, A>::insert(T* where, size_type n, T const& value)
{
    if (n == 0)
        return;

    if (max_size() - size() < n)
        ThrowLengthError("SmallVector::insert grew too long");

    if (n + size() > capacity())
    {
        size_type const newCapacity = new_capacity(size() + n);
        pointer newAllocation = mAllocator.allocate(newCapacity);
        size_type const whereOffset = where - mFirst;
        int stage = 0;
        try
        {
            std::uninitialized_fill(newAllocation + whereOffset, newAllocation + whereOffset + n, value);
            stage++;
            UninitializedMove(mFirst, where, newAllocation);
            stage++;
            UninitializedMove(where, mLast, newAllocation + whereOffset + n);
        }
        catch (...)
        {
            if (stage > 1)
                Destroy(newAllocation, newAllocation + whereOffset, mAllocator);
            if (stage > 0)
                Destroy(newAllocation + whereOffset, newAllocation + whereOffset + n, mAllocator);
            mAllocator.deallocate(newAllocation, newCapacity);
            throw;
        }

        Destroy(mFirst, mLast, mAllocator);
        if (mFirst != this->GetStorage())
            mAllocator.deallocate(mFirst, mEnd - mFirst);

        size_type const newSize = size() + n;
        mFirst = newAllocation;
        mLast = newAllocation + newSize;
        mEnd = newAllocation + newCapacity;
    }
    else
    {
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
                Destroy(where + n, end() + n, mAllocator);
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
        mLast += n;
    }
}

template<typename T, std::size_t S, typename A>
template<typename InputIt>
void SmallVector<T, S, A>::insert(T* where, InputIt first, InputIt last)
{
    insert_impl(where, first, last, typename std::iterator_traits<InputIt>::iterator_category());
}

template<typename T, std::size_t S, typename A>
template<typename InputIt>
void SmallVector<T, S, A>::insert_impl(T* where, InputIt first, InputIt last, std::input_iterator_tag)
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

template<typename T, std::size_t S, typename A>
template<typename InputIt>
void SmallVector<T, S, A>::insert_impl(T* where, InputIt first, InputIt last, std::forward_iterator_tag)
{
    auto const count = static_cast<size_type>(std::distance(first, last));
    if (count == 0)
        return;

    if (max_size() - size() < count)
        ThrowLengthError("SmallVector::insert grew too long");

    if (capacity() < size() + count)
    {
        size_type const newCapacity = new_capacity(size() + count);
        pointer newAllocation = mAllocator.allocate(newCapacity);
        pointer current = newAllocation;

        try
        {
            current = UninitializedMove(mFirst, where, newAllocation);
            current = std::uninitialized_copy(first, last, current);
            UninitializedMove(where, mLast, current);
        }
        catch (...)
        {
            Destroy(newAllocation, current, mAllocator);
            mAllocator.deallocate(newAllocation, newCapacity);
            throw;
        }

        Destroy(mFirst, mLast, mAllocator);
        if (mFirst != this->GetStorage())
            mAllocator.deallocate(mFirst, mEnd - mFirst);

        size_type const newSize = size() + count;
        mFirst = newAllocation;
        mLast = newAllocation + newSize;
        mEnd = newAllocation + newCapacity;
    }
    else
    {
        std::uninitialized_copy(first, last, end());
        std::rotate(where, end(), end() + count);
        mLast += count;
    }
}

template<typename T, std::size_t S, typename A>
T* SmallVector<T, S, A>::erase(T* where)
{
    Move(where + 1, mLast, where);
    mAllocator.destroy(--mLast);
    return where;
}

template<typename T, std::size_t S, typename A>
T* SmallVector<T, S, A>::erase(T* first, T* last)
{
    if (first == last)
        return first;

    T* newEnd = Move(last, mLast, first);
    Destroy(newEnd, mLast, mAllocator);
    mLast = newEnd;
    return first;
}

template<typename T, std::size_t S, typename A>
void SmallVector<T, S, A>::swap(SmallVector<T, S, A>& rhs)
{
    if (this == &rhs)
        return;

    if (mAllocator == rhs.mAllocator &&
        mFirst != this->GetStorage() &&
        rhs.mFirst != rhs.GetStorage())
    {
        std::swap(mFirst, rhs.mFirst);
        std::swap(mLast, rhs.mLast);
        std::swap(mEnd, rhs.mEnd);
    }
    else
    {
        ThisType temp(std::move(*this));
        *this = std::move(rhs);
        rhs = std::move(temp);
    }
}

template<typename T, std::size_t S, typename A>
void SmallVector<T, S, A>::clear()
{
    Destroy(mFirst, mLast, mAllocator);
    mLast = mFirst;
}

}}

#endif
