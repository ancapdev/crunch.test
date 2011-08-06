// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_TEST_FRAMEWORK_HPP
#define CRUNCH_TEST_FRAMEWORK_HPP

#include "crunch/base/platform.hpp"

#if defined (CRUNCH_COMPILER_MSVC)
#   pragma warning (push)
#   pragma warning (disable : 6326)
#endif

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

#if defined (CRUNCH_COMPILER_MSVC)
#   pragma warning (pop)
#endif

#endif
