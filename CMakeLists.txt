# Copyright (c) 2011-2013, Christian Rorvik
# Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

vpm_depend_self()

vpm_add_library(crunch_test_lib
  include/crunch/test/framework.hpp)

set_target_properties(crunch_test_lib PROPERTIES LINKER_LANGUAGE CXX)

