// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_CONTAINERS_EXCEPTION_HPP
#define CRUNCH_CONTAINERS_EXCEPTION_HPP

#include "crunch/base/noreturn.hpp"

namespace Crunch { namespace Containers {

CRUNCH_NORETURN void ThrowInvalidArgument(char const* message);
CRUNCH_NORETURN void ThrowLengthError(char const* message);
CRUNCH_NORETURN void ThrowOutOfRange(char const* message);
CRUNCH_NORETURN void ThrowOverflowError(char const* message);
CRUNCH_NORETURN void ThrowRuntimeError(char const* message);

}}

#endif
