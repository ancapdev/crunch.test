#ifndef CRUNCH_BASE_BIT_UTILITY_HPP
#define CRUNCH_BASE_BIT_UTILITY_HPP

#include "crunch/base/stdint.hpp"

namespace Crunch {

// Extract bits [first:last]
template<typename T>
T ExtractBits(T value, uint8 first, uint8 last)
{
    return (value & ((1 << (last + 1)) - 1)) >> first;
}

}

#endif
