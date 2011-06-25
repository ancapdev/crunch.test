// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_BASE_BIT_UTILITY_HPP
#define CRUNCH_BASE_BIT_UTILITY_HPP

#include "crunch/base/stdint.hpp"

namespace Crunch {

// Extract bits [first:last]
template<typename T>
T ExtractBits(T value, uint8 first, uint8 last)
{
    if (last >= sizeof(T) * 8 - 1)
        return value >> first;
    else
        return (value & ((T(1) << (last + 1)) - T(1))) >> first;
}

template<typename T>
T CountLeadingZeros(T value)
{
    if (value == 0)
        return sizeof(T) * 8;

    T count = 0;
    while ((value & (T(1) << (sizeof(T) * 8 - 1))) == 0)
    {
        value <<= 1;
        count++;
    }

    return count;
}

// Undefined for 0
template<typename T>
T Log2Floor(T value)
{
    return (sizeof(T) * 8 - 1) - CountLeadingZeros(value);
}

// Undefined for 0
template<typename T>
T Log2Ceil(T value)
{
    return sizeof(T) * 8 - CountLeadingZeros(value - 1);
}

template<typename T>
T Pow2Floor(T value)
{
    if (value == 0)
        return 0;
    else
        return T(1) << ((sizeof(T) * 8 - 1) - CountLeadingZeros(value));
}

template<typename T>
T Pow2Ceil(T value)
{
    if (value == 0)
        return 0;
    else
        return T(1) << (sizeof(T) * 8 - CountLeadingZeros(value - 1));
}

}

#endif
