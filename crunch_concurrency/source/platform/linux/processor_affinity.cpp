#include "crunch/concurrency/processor_affinity.hpp"
#include "crunch/base/assert.hpp"

#include <sched.h>

namespace Crunch { namespace Concurrency {

namespace
{
    cpu_set_t CreateCpuSet(ProcessorAffinity const& affinity)
    {
        cpu_set_t set;
        CPU_ZERO(&set);

        uint32 highest = affinity.GetHighestSetProcessor();
        CPU_SET(highest, &set);

        for (uint32 p = 0; p < highest; ++p)
        {
            if (affinity.IsSet(p))
                CPU_SET(p, &set);
        }

        return set;
    }

    ProcessorAffinity CreateAffinity(cpu_set_t* set)
    {
        ProcessorAffinity affinity;

        for (int p = 0; p < sizeof(cpu_set_t); ++p)
        {
            if (CPU_ISSET(p, set))
                affinity.Set(static_cast<uint32>(p));
        }

        return affinity;
    }
}

ProcessorAffinity SetThreadAffinity(ThreadId thread, ProcessorAffinity const& affinity)
{
    ProcessorAffinity old = GetProcessAffinity(thread);

    cpu_set_t set = CreateCpuSet(affinity);
    int result = sched_setaffinity(thread, sizeof(set), &set);
    CRUNCH_ASSERT_ALWAYS(result == 0);

    return old;
}

void SetProcessAffinity(ProcessId process, ProcessorAffinity const& affinity)
{
    cpu_set_t set = CreateCpuSet(affinity);
    int result = sched_setaffinity(process, sizeof(set), &set);
    CRUNCH_ASSERT_ALWAYS(result == 0);
}

ProcessorAffinity GetProcessAffinity(ProcessId process)
{
    cpu_set_t set;
    int result = sched_getaffinity(process, sizeof(set), &set);
    CRUNCH_ASSERT_ALWAYS(result == 0);
    return CreateAffinity(&set);
}

ProcessorAffinity SetCurrentThreadAffinity(ProcessorAffinity const& affinity)
{
    return SetThreadAffinity(0, affinity);
}

void SetCurrentProcessAffinity(ProcessorAffinity const& affinity)
{
    SetProcessAffinity(0, affinity);
}

ProcessorAffinity GetCurrentProcessAffinity()
{
    return GetProcessAffinity(0);
}

}}
