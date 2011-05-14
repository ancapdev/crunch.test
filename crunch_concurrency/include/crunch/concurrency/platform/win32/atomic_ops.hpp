#ifndef CRUNCH_CONCURRENCY_PLATFORM_WIN32_ATOMIC_OPS_HPP
#define CRUNCH_CONCURRENCY_PLATFORM_WIN32_ATOMIC_OPS_HPP

#include "crunch/base/assert.hpp"
#include "crunch/concurrency/fence.hpp"
#include "crunch/concurrency/memory_order.hpp"

#include <intrin.h>

#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/less_equal.hpp>
#include <boost/mpl/size_t.hpp>
#include <boost/mpl/sizeof.hpp>
#include <boost/utility/enable_if.hpp>

namespace Crunch { namespace Concurrency {

namespace Detail
{
    template<typename T, typename Enabler = void>
    struct AtomicLoadHelper;

    template<typename T, typename Enabler = void>
    struct AtomicStoreHelper;

    template<typename T, typename Enabler = void>
    struct AtomicArithmeticHelper;

    template<typename T, typename Enabler = void>
    struct AtomicSwapHelper;

    template<typename T, typename Enabler = void>
    struct AtomicCompareAndSwapHelper;

    template<typename T, typename Enabler = void>
    struct AtomicCompareAndSwapTestHelper
    {
        static bool Cas(T volatile& dst, T src, T cmp, MemoryOrder ordering)
        {
            return AtomicCompareAndSwapHelper<T>::Cas(dst, src, cmp, ordering) == cmp;
        }
    };

#if defined (CRUNCH_ARCH_X86)
    template<typename T>
    struct AtomicLoadHelper<T, typename boost::enable_if<boost::mpl::less_equal<boost::mpl::sizeof_<T>, boost::mpl::sizeof_<void*>>>::type>
    {
        static T Load(T volatile const& src, MemoryOrder ordering)
        {
            if (ordering == MEMORY_ORDER_SEQ_CST)
                CRUNCH_COMPILER_FENCE();

            T result = src;
            
            if (ordering == MEMORY_ORDER_ACQUIRE ||
                ordering == MEMORY_ORDER_SEQ_CST)
                CRUNCH_COMPILER_FENCE();
            
            return result;
        }
    };

    template<typename T>
    struct AtomicLoadHelper<T, typename boost::enable_if<boost::mpl::equal_to<boost::mpl::sizeof_<T>, boost::mpl::size_t<16>>>::type>
    {
        static T Load(T volatile const& src, MemoryOrder ordering)
        {
            if (ordering == MEMORY_ORDER_SEQ_CST)
                CRUNCH_COMPILER_FENCE();

            union
            {
                T value;
                __m128i sse;
            } result;

            result.sse = _mm_load_si128(reinterpret_cast<__m128i const*>(const_cast<T const*>(&src)));

            if (ordering == MEMORY_ORDER_ACQUIRE ||
                ordering == MEMORY_ORDER_SEQ_CST)
                CRUNCH_COMPILER_FENCE();

            return result.value;
        }
    };

    template<typename T, std::size_t S=sizeof(T)>
    struct AtomicStoreSeqCstHelper
    {
        static void Store(T volatile& dst, T src)
        {
            CRUNCH_MEMORY_FENCE();
            dst = src;
            CRUNCH_MEMORY_FENCE();
        }
    };
    
    template<typename T>
    struct AtomicStoreSeqCstHelper<T, 4>
    {
        static void Store(T volatile& dst, T src)
        {
            AtomicSwap(dst, src);
        }
    };

    template<typename T>
    struct AtomicStoreHelper<T, typename boost::enable_if<boost::mpl::less_equal<boost::mpl::sizeof_<T>, boost::mpl::sizeof_<void*>>>::type>
    {
        static void Store(T volatile& dst, T src, MemoryOrder ordering)
        {
            if (ordering == MEMORY_ORDER_SEQ_CST)
            {
                AtomicStoreSeqCstHelper<T>::Store(dst, src);
            }
            else
            {
                if (ordering == MEMORY_ORDER_RELEASE)
                    CRUNCH_COMPILER_FENCE();

                dst = src;
            
                if (ordering == MEMORY_ORDER_ACQUIRE)
                    CRUNCH_COMPILER_FENCE();
            }
        }
    };

    template<typename T>
    struct AtomicArithmeticHelper<T, typename boost::enable_if<boost::mpl::equal_to<boost::mpl::sizeof_<T>, boost::mpl::size_t<1>>>::type>
    {
        static T And(T volatile& value, T mask, MemoryOrder)
        {
            return (T)_InterlockedAnd8((char volatile*)&value, (char)mask);
        }

        static T Or(T volatile& value, T mask, MemoryOrder)
        {
            return (T)_InterlockedOr8((char volatile*)&value, (char)mask);
        }

        static T Xor(T volatile& value, T mask, MemoryOrder)
        {
            return (T)_InterlockedXor8((char volatile*)&value, (char)mask);
        }
    };

    template<typename T>
    struct AtomicArithmeticHelper<T, typename boost::enable_if<boost::mpl::equal_to<boost::mpl::sizeof_<T>, boost::mpl::size_t<2>>>::type>
    {
        static T Increment(T volatile& addend, MemoryOrder)
        {
            return (T)_InterlockedIncrement16((short volatile*)&addend) - 1;
        }

        static T Decrement(T volatile& addend, MemoryOrder)
        {
            return (T)_InterlockedDecrement16((short volatile*)&addend) + 1;
        }

        static T And(T volatile& value, T mask, MemoryOrder)
        {
            return (T)_InterlockedAnd16((short volatile*)&value, (short)mask);
        }

        static T Or(T volatile& value, T mask, MemoryOrder)
        {
            return (T)_InterlockedOr16((short volatile*)&value, (short)mask);
        }

        static T Xor(T volatile& value, T mask, MemoryOrder)
        {
            return (T)_InterlockedXor16((short volatile*)&value, (short)mask);
        }
    };

    template<typename T>
    struct AtomicArithmeticHelper<T, typename boost::enable_if<boost::mpl::equal_to<boost::mpl::sizeof_<T>, boost::mpl::size_t<4>>>::type>
    {
        static T Add(T volatile& addend, T value, MemoryOrder)
        {
            return (T)_InterlockedExchangeAdd((long volatile*)&addend, (long)value);
        }

        static T Increment(T volatile& addend, MemoryOrder)
        {
            return (T)_InterlockedIncrement((long volatile*)&addend) - 1;
        }

        static T Decrement(T volatile& addend, MemoryOrder)
        {
            return (T)_InterlockedDecrement((long volatile*)&addend) + 1;
        }

        static T And(T volatile& value, T mask, MemoryOrder)
        {
            return (T)_InterlockedAnd((long volatile*)&value, (long)mask);
        }

        static T Or(T volatile& value, T mask, MemoryOrder)
        {
            return (T)_InterlockedOr((long volatile*)&value, (long)mask);
        }

        static T Xor(T volatile& value, T mask, MemoryOrder)
        {
            return (T)_InterlockedXor((long volatile*)&value, (long)mask);
        }
    };

    template<typename T>
    struct AtomicSwapHelper<T, typename boost::enable_if<boost::mpl::equal_to<boost::mpl::sizeof_<T>, boost::mpl::size_t<4>>>::type>
    {
        static T Swap(T volatile& dst, T src, MemoryOrder)
        {
            return (T)_InterlockedExchange((long volatile*)&dst, (long)src);
        }
    };

    template<typename T>
    struct AtomicCompareAndSwapHelper<T, typename boost::enable_if<boost::mpl::equal_to<boost::mpl::sizeof_<T>, boost::mpl::size_t<2>>>::type>
    {
        static T Cas(T volatile& dst, T src, T cmp, MemoryOrder)
        {
            return (T)_InterlockedCompareExchange16((short volatile*)&dst, (short)src, (short)cmp);
        }
    };
    
    template<typename T>
    struct AtomicCompareAndSwapHelper<T, typename boost::enable_if<boost::mpl::equal_to<boost::mpl::sizeof_<T>, boost::mpl::size_t<4>>>::type>
    {
        static T Cas(T volatile& dst, T src, T cmp, MemoryOrder)
        {
            return (T)_InterlockedCompareExchange((long volatile*)&dst, (long)src, (long)cmp);
        }
    };

    template<typename T>
    struct AtomicCompareAndSwapHelper<T, typename boost::enable_if<boost::mpl::equal_to<boost::mpl::sizeof_<T>, boost::mpl::size_t<8>>>::type>
    {
        static T Cas(T volatile& dst, T src, T cmp, MemoryOrder)
        {
            return (T)_InterlockedCompareExchange64((__int64 volatile*)&dst, (__int64)src, (__int64)cmp);
        }
    };
#endif


#if defined (CRUNCH_ARCH_X86_32)
    template<typename T>
    struct AtomicLoadHelper<T, typename boost::enable_if<boost::mpl::equal_to<boost::mpl::sizeof_<T>, boost::mpl::size_t<8>>>::type>
    {
        static T Load(T volatile const& src, MemoryOrder ordering)
        {
            if (ordering == MEMORY_ORDER_SEQ_CST)
                CRUNCH_COMPILER_FENCE();

            T result;
            __asm
            {
                mov eax, dword ptr [src]
                fild qword ptr [eax]
                fistp qword ptr [result]
            }

            if (ordering == MEMORY_ORDER_ACQUIRE ||
                ordering == MEMORY_ORDER_SEQ_CST)
                CRUNCH_COMPILER_FENCE();

            return result;
        }
    };

    template<typename T>
    struct AtomicStoreHelper<T, typename boost::enable_if<boost::mpl::equal_to<boost::mpl::sizeof_<T>, boost::mpl::size_t<8>>>::type>
    {
        static void Store(T volatile& dst, T src, MemoryOrder ordering)
        {
            if (ordering == MEMORY_ORDER_SEQ_CST)
                CRUNCH_MEMORY_FENCE();
            else if (ordering == MEMORY_ORDER_RELEASE)
                CRUNCH_COMPILER_FENCE();

            __asm
            {
                mov eax, dword ptr [dst]
                fild qword ptr [src]
                fistp qword ptr [eax]
            }

            if (ordering == MEMORY_ORDER_SEQ_CST)
                CRUNCH_MEMORY_FENCE();
            else if (ordering == MEMORY_ORDER_ACQUIRE)
                CRUNCH_COMPILER_FENCE();
        }
    };
#endif

#if defined (CRUNCH_ARCH_X86_64)
    template<typename T>
    struct AtomicStoreSeqCstHelper<T, 8>
    {
        static void Store(T volatile& dst, T src)
        {
            AtomicSwap(dst, src);
        }
    };

    template<typename T>
    struct AtomicArithmeticHelper<T, typename boost::enable_if<boost::mpl::equal_to<boost::mpl::sizeof_<T>, boost::mpl::size_t<8>>>::type>
    {
        static T Add(T volatile& addend, T value, MemoryOrder)
        {
            return (T)_InterlockedExchangeAdd64((__int64 volatile*)&addend, (__int64)value);
        }

        static T Increment(T volatile& addend, MemoryOrder)
        {
            return (T)_InterlockedIncrement64((__int64 volatile*)&addend) - 1;
        }

        static T Decrement(T volatile& addend, MemoryOrder)
        {
            return (T)_InterlockedDecrement64((__int64 volatile*)&addend) + 1;
        }

        static T And(T volatile& value, T mask, MemoryOrder)
        {
            return (T)_InterlockedAnd64((__int64 volatile*)&value, (__int64)mask);
        }

        static T Or(T volatile& value, T mask, MemoryOrder)
        {
            return (T)_InterlockedOr64((__int64 volatile*)&value, (__int64)mask);
        }

        static T Xor(T volatile& value, T mask, MemoryOrder)
        {
            return (T)_InterlockedXor64((__int64 volatile*)&value, (__int64)mask);
        }
    };

    template<typename T>
    struct AtomicSwapHelper<T, typename boost::enable_if<boost::mpl::equal_to<boost::mpl::sizeof_<T>, boost::mpl::size_t<8>>>::type>
    {
        static T Swap(T volatile& dst, T src, MemoryOrder)
        {
            return (T)_InterlockedExchange64((__int64 volatile*)&dst, (__int64)src);
        }
    };

    template<typename T>
    struct AtomicCompareAndSwapHelper<T, typename boost::enable_if<boost::mpl::equal_to<boost::mpl::sizeof_<T>, boost::mpl::size_t<16>>>::type>
    {
        static T Cas(T volatile& dst, T src, T cmp, MemoryOrder)
        {
            _InterlockedCompareExchange128(
                (__int64 volatile*)&dst,
                ((__int64 const*)&src)[1],
                ((__int64 const*)&src)[0],
                (__int64*)&cmp);

            return cmp;
        }
    };

    template<typename T>
    struct AtomicCompareAndSwapTestHelper<T, typename boost::enable_if<boost::mpl::equal_to<boost::mpl::sizeof_<T>, boost::mpl::size_t<16>>>::type>
    {
        static bool Cas(T volatile& dst, T src, T cmp, MemoryOrder)
        {
            return _InterlockedCompareExchange128(
                (__int64 volatile*)&dst,
                ((__int64 const*)&src)[1],
                ((__int64 const*)&src)[0],
                (__int64*)&cmp) != 0;
        }
    };
#endif
}

template<typename T>
inline T AtomicLoad(T volatile& src, MemoryOrder ordering = MEMORY_ORDER_SEQ_CST)
{
    CRUNCH_ASSERT_ALIGNMENT(&src, sizeof(T));
    return Detail::AtomicLoadHelper<T>::Load(src, ordering);
}

template<typename T>
inline void AtomicStore(T volatile& dst, T src, MemoryOrder ordering = MEMORY_ORDER_SEQ_CST)
{
    CRUNCH_ASSERT_ALIGNMENT(&dst, sizeof(T));
    return Detail::AtomicStoreHelper<T>::Store(dst, src, ordering);
}

template<typename T>
T AtomicSwap(T volatile& dst, T src, MemoryOrder ordering = MEMORY_ORDER_SEQ_CST)
{
    CRUNCH_ASSERT_ALIGNMENT(&dst, sizeof(T));
    return Detail::AtomicSwapHelper<T>::Swap(dst, src, ordering);
}

template<typename T>
T AtomicCompareAndSwap(T volatile& dst, T src, T cmp, MemoryOrder ordering = MEMORY_ORDER_SEQ_CST)
{
    CRUNCH_ASSERT_ALIGNMENT(&dst, sizeof(T));
    return Detail::AtomicCompareAndSwapHelper<T>::Cas(dst, src, cmp, ordering);
}

template<typename T>
bool AtomicCompareAndSwapTest(T volatile& dst, T src, T cmp, MemoryOrder ordering = MEMORY_ORDER_SEQ_CST)
{
    CRUNCH_ASSERT_ALIGNMENT(&dst, sizeof(T));
    return Detail::AtomicCompareAndSwapTestHelper<T>::Cas(dst, src, cmp, ordering);
}

template<typename T>
inline T AtomicIncrement(T volatile& addend, MemoryOrder ordering = MEMORY_ORDER_SEQ_CST)
{
    CRUNCH_ASSERT_ALIGNMENT(&addend, sizeof(T));
    return Detail::AtomicArithmeticHelper<T>::Increment(addend, ordering);
}

template<typename T>
inline T AtomicDecrement(T volatile& addend, MemoryOrder ordering = MEMORY_ORDER_SEQ_CST)
{
    CRUNCH_ASSERT_ALIGNMENT(&addend, sizeof(T));
    return Detail::AtomicArithmeticHelper<T>::Decrement(addend, ordering);
}

template<typename T>
inline T AtomicAdd(T volatile& addend, T value, MemoryOrder ordering = MEMORY_ORDER_SEQ_CST)
{
    CRUNCH_ASSERT_ALIGNMENT(&addend, sizeof(T));
    return Detail::AtomicArithmeticHelper<T>::Add(addend, value, ordering);
}

template<typename T>
inline T AtomicAnd(T volatile& value, T mask, MemoryOrder ordering = MEMORY_ORDER_SEQ_CST)
{
    CRUNCH_ASSERT_ALIGNMENT(&value, sizeof(T));
    return Detail::AtomicArithmeticHelper<T>::And(value, mask, ordering);
}

template<typename T>
inline T AtomicOr(T volatile& value, T mask, MemoryOrder ordering = MEMORY_ORDER_SEQ_CST)
{
    CRUNCH_ASSERT_ALIGNMENT(&value, sizeof(T));
    return Detail::AtomicArithmeticHelper<T>::Or(value, mask, ordering);
}

template<typename T>
inline T AtomicXor(T volatile& value, T mask, MemoryOrder ordering = MEMORY_ORDER_SEQ_CST)
{
    CRUNCH_ASSERT_ALIGNMENT(&value, sizeof(T));
    return Detail::AtomicArithmeticHelper<T>::Xor(value, mask, ordering);
}

}}

#endif
