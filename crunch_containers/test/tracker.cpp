// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "./tracker.hpp"

#include <ostream>

namespace Crunch { namespace Containers {

std::ostream& operator << (std::ostream& os, Tracker const& tracker)
{
    return os << "(" << &tracker.GetStatistics() << ", " << tracker.GetTag() << ")";
}

}}
