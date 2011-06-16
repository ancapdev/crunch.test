#include "crunch/concurrency/yield.hpp"

#include <windows.h>

namespace Crunch { namespace Concurrency {

void ThreadYield()
{
    SwitchToThread();
}

}}
