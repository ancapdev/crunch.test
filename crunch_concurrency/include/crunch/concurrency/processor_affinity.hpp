#ifndef CRUNCH_CONCURRENCY_PROCESSOR_AFFINITY_HPP
#define CRUNCH_CONCURRENCY_PROCESSOR_AFFINITY_HPP

#include "crunch/base/stdint.hpp"
#include "crunch/concurrency/thread.hpp"
#include "crunch/concurrency/processor_topology.hpp"

#include <bitset>

namespace Crunch { namespace Concurrency {

class ProcessorAffinity
{
public:
    ProcessorAffinity();
    ProcessorAffinity(uint32 processorId);
    ProcessorAffinity(ProcessorTopology::LogicalProcessor const& processor);
    ProcessorAffinity(ProcessorTopology::PhysicalProcessor const& processor);
    ProcessorAffinity(ProcessorTopology::Package const& package);
    
    void Set(uint32 processorId);
    void Set(ProcessorTopology::LogicalProcessor const& processor);
    void Set(ProcessorTopology::PhysicalProcessor const& processor);
    void Set(ProcessorTopology::Package const& package);

    void Clear(uint32 processorId);
    void Clear(ProcessorTopology::LogicalProcessor const& processor);
    void Clear(ProcessorTopology::PhysicalProcessor const& processor);
    void Clear(ProcessorTopology::Package const& package);

    bool IsSet(uint32 processorId) const;

    uint32 GetHighestSetProcessor() const;

private:
    static const uint32 MaxProcessorId = 31;
    typedef std::bitset<MaxProcessorId+1> MaskType;
    MaskType mProcessorMask;
};

ProcessorAffinity operator ~ (ProcessorAffinity affinity);
ProcessorAffinity operator | (ProcessorAffinity lhs, ProcessorAffinity rhs);
ProcessorAffinity operator & (ProcessorAffinity lhs, ProcessorAffinity rhs);
ProcessorAffinity operator ^ (ProcessorAffinity lhs, ProcessorAffinity rhs);
ProcessorAffinity& operator |= (ProcessorAffinity& lhs, ProcessorAffinity rhs);
ProcessorAffinity& operator &= (ProcessorAffinity& lhs, ProcessorAffinity rhs);
ProcessorAffinity& operator ^= (ProcessorAffinity& lhs, ProcessorAffinity rhs);


/// \return Old affinity
ProcessorAffinity SetThreadAffinity(ThreadId thread, ProcessorAffinity const& affinity);
void SetProcessAffinity(ProcessId process, ProcessorAffinity const& affinity);
ProcessorAffinity GetProcessAffinity(ProcessId process);

ProcessorAffinity SetCurrentThreadAffinity(ProcessorAffinity const& affinity);
void SetCurrentProcessAffinity(ProcessorAffinity const& affinity);
ProcessorAffinity GetCurrentProcessAffinity();

}}

#endif
