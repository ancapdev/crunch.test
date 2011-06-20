#include "crunch/concurrency/detail/future_data.hpp"

namespace Crunch { namespace Concurrency { namespace Detail {

void FutureDataBase::Destroy()
{
    delete this;
}

}}}
