#include "crunch/concurrency/meta_scheduler.hpp"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

namespace Crunch { namespace Concurrency {

BOOST_AUTO_TEST_SUITE(MetaSchedulerTests)

BOOST_AUTO_TEST_CASE(WaitTest)
{
    struct NullWaitable : IWaitable
    {
        virtual void AddWaiter(Waiter* waiter) { waiter->Notify(); }
        virtual void RemoveWaiter(Waiter* waiter) {}
        virtual bool IsOrderDependent() const { return false; }
    };

    NullWaitable nullWaitable;
    IWaitable* waitables[] = { &nullWaitable };

    MetaScheduler::WaitForAll(waitables, MetaScheduler::WAIT_MODE_YIELD_PREEMTIVE);

    MetaScheduler::WaitForAny(waitables, MetaScheduler::WAIT_MODE_YIELD_PREEMTIVE);
}

BOOST_AUTO_TEST_SUITE_END()

}}
