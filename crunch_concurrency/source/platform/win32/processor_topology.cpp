#include "crunch/concurrency/processor_topology.hpp"

#include <windows.h>

namespace Crunch { namespace Concurrency {

uint32 GetSystemNumProcessors()
{
    SYSTEM_INFO info;
    GetSystemInfo(info);
    return info.dwNumberOfProcessors;
}

}}
