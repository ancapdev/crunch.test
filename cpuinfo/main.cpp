#include "crunch/concurrency/processor_topology.hpp"

#include <algorithm>
#include <iostream>

int main()
{
    using namespace Crunch::Concurrency;

    ProcessorTopology topology;

    ProcessorTopology::ProcessorList const& processors = topology.GetProcessors();

    std::cout << "Id | Thread | Core | Package" << std::endl;
    std::cout << "---+--------+------+---------" << std::endl;
    std::for_each(processors.begin(), processors.end(), [](ProcessorTopology::Processor const& p) {
        std::cout << " " << p.systemId << " |      " << p.threadId << " |    " << p.coreId << " |       " << p.packageId << std::endl;
    });

    return 0;
}
