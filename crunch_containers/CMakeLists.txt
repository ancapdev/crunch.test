# Copyright (c) 2011, Christian Rorvik
# Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

add_library(crunch_containers_lib
  include/crunch/containers/exception.hpp
  include/crunch/containers/fixed_vector.hpp
  include/crunch/containers/small_vector.hpp
  include/crunch/containers/utility.hpp
  include/crunch/containers/detail/fixed_vector_base.hpp
  source/exception.cpp)
  
set_target_properties(crunch_containers_lib PROPERTIES LINKER_LANGUAGE CXX)

add_executable(crunch_containers_test
  test/fixed_vector_tests.cpp
  test/main.cpp
  test/small_vector_tests.cpp)

target_link_libraries(crunch_containers_test
  crunch_containers_lib)
  
crunch_add_test(crunch_containers_test)
