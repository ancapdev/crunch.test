#include "crunch/concurrency/yield.hpp"

#include <sched.h>

namespace Crunch { namespace Concurrency {

void ThreadYield()
{
    sched_yield();
}

}}
