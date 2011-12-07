# Copyright (c) 2011, Christian Rorvik
# Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

include(configure.cmake)

# Create 2 libraries to move the boost test library before crunch_test_lib_ 
# in the link order. Dependent tests should link with crunch_test_lib

vpm_add_library(crunch_test_lib_
  include/crunch/test/framework.hpp
  source/main.cpp)

vpm_add_library(crunch_test_lib
  source/dummy.cpp)

target_link_libraries(crunch_test_lib
  boost_test
  crunch_test_lib_)
