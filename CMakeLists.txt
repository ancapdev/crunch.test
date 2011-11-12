# Copyright (c) 2011, Christian Rorvik
# Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

include(configure.cmake)

vpm_add_dependencies(boost)

add_library(crunch_test_lib
  include/crunch/test/framework.hpp
  source/main.cpp)
