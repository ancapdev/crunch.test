#include "crunch/concurrency/processor_affinity.hpp"

#include <algorithm>

namespace Crunch { namespace Concurrency {

ProcessorAffinity::ProcessorAffinity()
{
}

ProcessorAffinity::ProcessorAffinity(uint32 processorId)
{
    Set(processorId);
}

ProcessorAffinity::ProcessorAffinity(ProcessorTopology::LogicalProcessor const& processor)
{
    Set(processor);
}

ProcessorAffinity::ProcessorAffinity(ProcessorTopology::PhysicalProcessor const& processor)
{
    Set(processor);
}

ProcessorAffinity::ProcessorAffinity(ProcessorTopology::Package const& package)
{
    Set(package);
}

void ProcessorAffinity::Set(uint32 processorId)
{
    mProcessorMask.set(processorId);
}

void ProcessorAffinity::Set(ProcessorTopology::LogicalProcessor const& processor)
{
    Set(processor.id);
}

void ProcessorAffinity::Set(ProcessorTopology::PhysicalProcessor const& processor)
{
    std::for_each(processor.logicalIds.begin(), processor.logicalIds.end(), [&] (uint32 id) { Set(id); });
}

void ProcessorAffinity::Set(ProcessorTopology::Package const& package)
{
    std::for_each(package.logicalIds.begin(), package.logicalIds.end(), [&] (uint32 id) { Set(id); });
}

void ProcessorAffinity::Clear(uint32 processorId)
{
    mProcessorMask.reset(processorId);
}

void ProcessorAffinity::Clear(ProcessorTopology::LogicalProcessor const& processor)
{
    mProcessorMask.reset(processor.id);
}

void ProcessorAffinity::Clear(ProcessorTopology::PhysicalProcessor const& processor)
{
    std::for_each(processor.logicalIds.begin(), processor.logicalIds.end(), [&] (uint32 id) { Clear(id); });
}

void ProcessorAffinity::Clear(ProcessorTopology::Package const& package)
{
    std::for_each(package.logicalIds.begin(), package.logicalIds.end(), [&] (uint32 id) { Clear(id); });
}

bool ProcessorAffinity::IsSet(uint32 processorId) const
{
    return mProcessorMask.test(processorId);
}

uint32 ProcessorAffinity::GetHighestSetProcessor() const
{
    for (uint32 i = MaxProcessorId; i <= MaxProcessorId; --i)
        if (IsSet(i))
            return i;

    // TODO: return optional none
    return 0;
}

}}
