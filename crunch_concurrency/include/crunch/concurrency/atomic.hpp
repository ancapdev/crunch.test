#ifndef CRUNCH_CONCURRENCY_ATOMIC_HPP
#define CRUNCH_CONCURRENCY_ATOMIC_HPP

#include "crunch/base/platform.hpp"

#if defined (CRUNCH_PLATFORM_WIN32)
#   include "crunch/concurrency/platform/win32/atomic_ops.hpp"
#endif

namespace Crunch { namespace Concurrency {

template<typename T>
class Atomic
{
};

}}

#endif
