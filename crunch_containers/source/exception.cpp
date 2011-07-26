// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/containers/exception.hpp"
#include <stdexcept>

namespace Crunch { namespace Containers {

void ThrowInvalidArgument(char const* message)
{
    throw std::invalid_argument(message);
}

void ThrowLengthError(char const* message)
{
    throw std::length_error(message);
}

void ThrowOutOfRange(char const* message)
{
    throw std::out_of_range(message);
}

void ThrowOverflowError(char const* message)
{
    throw std::overflow_error(message);
}

void ThrowRuntimeError(char const* message)
{
    throw std::runtime_error(message);
}

}}
