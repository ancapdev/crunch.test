#include "crunch/concurrency/processor_topology.hpp"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>


// TODO: remove
#include <iostream>

namespace Crunch { namespace Concurrency {

BOOST_AUTO_TEST_SUITE(ProcessorTopologyTests)

BOOST_AUTO_TEST_CASE(RemoveMe)
{
    ProcessorTopology topology;

    ProcessorTopology::ProcessorList const& processors = topology.GetProcessors();

    std::cout << "Id | Thread | Core | Package" << std::endl;
    std::cout << "---+--------+------+---------" << std::endl;
    std::for_each(processors.begin(), processors.end(), [](ProcessorTopology::Processor const& p) {
        std::cout << " " << p.systemId << " |      " << p.threadId << " |    " << p.coreId << " |       " << p.packageId << std::endl;
    });

}

BOOST_AUTO_TEST_SUITE_END()

}}
