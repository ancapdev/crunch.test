# Copyright (c) 2011, Christian Rorvik
# Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

if (Boost_FOUND)
  add_library(crunch_test_lib
    include/crunch/test/framework.hpp
    source/main.cpp)
endif()
