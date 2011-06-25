#ifndef CRUNCH_CONCURRENCY_PROCESSOR_TOPOLOGY_HPP
#define CRUNCH_CONCURRENCY_PROCESSOR_TOPOLOGY_HPP

#include "crunch/base/stdint.hpp"

#include <vector>

namespace Crunch { namespace Concurrency {

uint32 GeSystemtNumProcessors();

class ProcessorTopology
{
public:
    struct LogicalProcessor
    {
        uint32 id;
        uint32 physicalId;
        uint32 packageId;
    };

    struct PhysicalProcessor
    {
        uint32 id;
        uint32 packageId;
        std::vector<uint32> logicalIds;
    };

    struct Package
    {
        uint32 id;
        std::vector<uint32> physicalIds;
        std::vector<uint32> logicalIds;
    };

    typedef std::vector<LogicalProcessor> LogicalProcessorList;
    typedef std::vector<PhysicalProcessor> PhysicalProcessorList;
    typedef std::vector<Package> PackageList;

    ProcessorTopology();

    LogicalProcessorList const& GetLogicalProcessors() const { return mLogicalProcessors; }
    PhysicalProcessorList const& GetPhysicalProcessors() const { return mPhysicalProcessors; }
    PackageList const& GetPackages() const { return mPackages; }

private:
    LogicalProcessorList mLogicalProcessors;
    PhysicalProcessorList mPhysicalProcessors;
    PackageList mPackages;
};

}}

#endif

