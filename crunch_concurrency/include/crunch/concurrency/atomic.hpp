#ifndef CRUNCH_CONCURRENCY_ATOMIC_HPP
#define CRUNCH_CONCURRENCY_ATOMIC_HPP

#include "crunch/base/platform.hpp"

#if defined (CRUNCH_PLATFORM_WIN32)
#   include "crunch/concurrency/platform/win32/atomic.hpp"
#endif

#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_integral.hpp>

namespace Crunch { namespace Concurrency {

template<typename T, typename W = typename Platform::AtomicWord<T>::Type>
class AtomicBase
{
public:
    typedef T ValueType;
    typedef W WordType;
    typedef Platform::AtomicStorage<WordType> StorageType;

    void Store(ValueType value, MemoryOrder ordering = MEMORY_ORDER_SEQ_CST) volatile
    {
        Converter c;
        c.value =  value;
        Platform::AtomicStore(mData.bits, c.bits, ordering);
    }

    ValueType Load(MemoryOrder ordering =  MEMORY_ORDER_SEQ_CST) const volatile
    {
        Converter result;
        result.bits = Platform::AtomicLoad(mData.bits, ordering);
        return result.value;
    }

    ValueType Swap(ValueType src, MemoryOrder ordering = MEMORY_ORDER_SEQ_CST) volatile
    {
        Converter src_;
        src_.value = src;

        Converter result;
        result.bits = Platform::AtomicSwap(mData.bits, src_.bits, ordering);
        return result.value;
    }

    bool CompareAndSwap(ValueType src, ValueType& cmp, MemoryOrder ordering = MEMORY_ORDER_SEQ_CST) volatile
    {
        Converter src_;
        src_.value = src;

        Converter cmp_;
        cmp_.value = cmp;

        bool const result = Platform::AtomicCompareAndSwap(mData.bits, src_.bits, cmp_.bits, ordering);
        cmp = cmp_.value;
        return result;
    }

    bool CompareAndSwap(ValueType src, WordType& cmp, MemoryOrder ordering = MEMORY_ORDER_SEQ_CST) volatile
    {
        Converter src_;
        src_.value = src;

        return Platform::AtomicCompareAndSwap(mData.bits, src_.bits, cmp, ordering);
    }

    operator ValueType () const volatile
    {
        return Load();
    }

    ValueType operator=(ValueType value) volatile
    {
        Store();
        return value;
    }

protected:
    union Converter
    {
        WordType bits;
        ValueType value;
    };

    StorageType mData;
};


template<typename T>
class AtomicBase<T, T>
{
public:
    typedef T ValueType;
    typedef T WordType;
    typedef Platform::AtomicStorage<WordType> StorageType;


    void Store(ValueType value, MemoryOrder ordering = MEMORY_ORDER_SEQ_CST) volatile
    {
        Platform::AtomicStore(mData.bits, value, ordering);
    }

    ValueType Load(MemoryOrder ordering =  MEMORY_ORDER_SEQ_CST) const volatile
    {
        return Platform::AtomicLoad(mData.bits, ordering);
    }

    ValueType Swap(ValueType src, MemoryOrder ordering = MEMORY_ORDER_SEQ_CST) volatile
    {
        return Platform::AtomicSwap(mData.bits, src, ordering);
    }

    bool CompareAndSwap(ValueType src, ValueType& cmp, MemoryOrder ordering = MEMORY_ORDER_SEQ_CST) volatile
    {
        return Platform::AtomicCompareAndSwap(mData.bits, src, cmp, ordering);
    }

    operator ValueType () const volatile
    {
        return Load();
    }

    ValueType operator=(ValueType value) volatile
    {
        Store();
        return value;
    }

protected:
    StorageType mData;
};


template<typename T, typename Enabler = void>
class Atomic : public AtomicBase<T>
{
public:
    typedef Atomic<T> ThisType;

    Atomic() {};

    Atomic(ValueType value, MemoryOrder ordering = MEMORY_ORDER_SEQ_CST)
    {
        Store(value,  ordering);
    }
};

template<typename T, typename W = typename Platform::AtomicWord<T>::Type>
class AtomicIntegerBase : public AtomicBase<T, W>
{
public:
    ValueType Increment() volatile
    {
        Converter result;
        result.bits = Platform::AtomicIncrement(mData.bits);
        return result.value;
    }

    ValueType Decrement() volatile
    {
        Converter result;
        result.bits = Platform::AtomicDecrement(mData.bits);
        return result.value;
    }

    ValueType Add(ValueType value) volatile
    {
        Converter value_;
        value_.value = value;

        Converter result;
        result.bits = Platform::AtomicAdd(mData.bits, value_.bits);
        return result.value;
    }

    ValueType Sub(ValueType value) volatile
    {
        Converter value_;
        value_.value = value;

        Converter result;
        result.bits = Platform::AtomicSub(mData.bits, value_.bits);
        return result.value;
    }

    ValueType And(ValueType mask) volatile
    {
        Converter mask_;
        mask_.value = mask;

        Converter result;
        result.bits = Platform::AtomicAnd(mData.bits, mask_.bits);
        return result.value;
    }

    ValueType Or(ValueType mask) volatile
    {
        Converter mask_;
        mask_.value = mask;

        Converter result;
        result.bits = Platform::AtomicOr(mData.bits, mask_.bits);
        return result.value;
    }

    ValueType Xor(ValueType mask) volatile
    {
        Converter mask_;
        mask_.value = mask;

        Converter result;
        result.bits = Platform::AtomicXor(mData.bits, mask_.bits);
        return result.value;
    }
};

template<typename T>
class AtomicIntegerBase<T, T> : public AtomicBase<T, T>
{
public:
    ValueType Increment() volatile
    {
        return Platform::AtomicIncrement(mData.bits);
    }

    ValueType Decrement() volatile
    {
        return Platform::AtomicDecrement(mData.bits);
    }

    ValueType Add(ValueType value) volatile
    {
        return Platform::AtomicAdd(mData.bits, value);
    }

    ValueType Sub(ValueType value) volatile
    {
        return Platform::AtomicSub(mData.bits, value);
    }

    ValueType And(ValueType mask) volatile
    {
        return Platform::AtomicAnd(mData.bits, mask);
    }

    ValueType Or(ValueType mask) volatile
    {
        return Platform::AtomicOr(mData.bits, mask);
    }

    ValueType Xor(ValueType mask) volatile
    {
        return Platform::AtomicXor(mData.bits, mask);
    }
};


template<typename T>
class Atomic<T, typename boost::enable_if<boost::is_integral<T>>::type> : public AtomicIntegerBase<T>
{
public:
    Atomic() {}

    Atomic(ValueType value, MemoryOrder ordering = MEMORY_ORDER_SEQ_CST)
    {
        Store(value,  ordering);
    }

    ValueType operator++() volatile
    {
        return Increment() + 1;
    }

    ValueType operator++(int)  volatile
    {
        return Increment();
    }

    ValueType operator--() volatile
    {
        return Increment() - 1;
    }

    ValueType operator--(int)  volatile
    {
        return Increment();
    }

    ValueType operator += (ValueType value) volatile
    {
        return Add(value) + value;
    }

    ValueType operator -= (ValueType value) volatile
    {
        return Sub(value) - value;
    }
};

template<typename T>
class Atomic<T*> : public AtomicBase<T*>
{
    T* Increment() volatile
    {
        return Add(1);
    }

    T* Decrement() volatile
    {
        return Sub(1);
    }

    T* Add(std::ptrdiff_t value) volatile
    {
        Converter value_;
        value_.value = value * sizeof(T);

        Converter result;
        result.bits = Platform::AtomicAdd(mData.bits, value_.bits);
        return result.value;
    }

    T* Sub(std::ptrdiff_t value) volatile
    {
        Converter value_;
        value_.value = value * sizeof(T);

        Converter result;
        result.bits = Platform::AtomicSub(mData.bits, value_.bits);
        return result.value;
    }

    T* operator++() volatile
    {
        return Increment() + 1;
    }

    T* operator++(int)  volatile
    {
        return Increment();
    }

    T* operator--() volatile
    {
        return Increment() - 1;
    }

    T* operator--(int)  volatile
    {
        return Increment();
    }

    T* operator += (std::ptrdiff_t value) volatile
    {
        return Add(value) + value;
    }

    T* operator -= (std::ptrdiff_t value) volatile
    {
        return Sub(value) - value;
    }
};


template<typename T>
T AtomicCompareAndSwap(Atomic<T> volatile& dst, T src, T cmp, MemoryOrder ordering = MEMORY_ORDER_SEQ_CST)
{
    return dst.CompareAndSwap(src, cmp, ordering);
}

}}

#endif
