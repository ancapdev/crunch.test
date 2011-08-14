# Copyright (c) 2011, Christian Rorvik
# Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

crunch_add_library(crunch_containers_lib
  include/crunch/containers/exception.hpp
  include/crunch/containers/fixed_vector.hpp
  include/crunch/containers/small_vector.hpp
  include/crunch/containers/utility.hpp
  include/crunch/containers/detail/fixed_vector_base.hpp
  source/exception.cpp)
  
if (Boost_FOUND)
  add_executable(crunch_containers_test
    test/fixed_vector_tests.cpp
    test/small_vector_tests.cpp
    test/tracker.cpp test/tracker.hpp)

  target_link_libraries(crunch_containers_test
    crunch_containers_lib
    crunch_test_lib)
    
  crunch_add_test(crunch_containers_test)
endif()
