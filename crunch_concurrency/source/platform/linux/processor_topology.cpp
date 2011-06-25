#include "crunch/concurrency/processor_topology.hpp"

#include <unistd.h>

namespace Crunch { namespace Concurrency {

uint32 GetSystemNumProcessors()
{
    long numProcessors = sysconf(_SC_NPROCESSORS_ONLN);
    if (numProcessors > 0)
        return static_cast<uint32>(numProcessors);
    else
        return 1;
}

}}

