// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

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
