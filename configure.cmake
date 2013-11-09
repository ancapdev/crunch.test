# Copyright (c) 2011-2013, Christian Rorvik
# Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

vpm_set_default_variant(boost proxy)
vpm_set_default_version(boost 1.53.0)
vpm_depend(boost)

vpm_include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

macro(crunch_add_test _name)
  # Shared build target for all tests
  if(NOT TARGET build-tests)
    add_custom_target(build-tests)
  endif()

  # Shared run target for all tests
  if(NOT TARGET run-tests)
    add_custom_target(run-tests)
  endif()

  # Run target for this test
  add_custom_target("run-${_name}"
    ALL
    ${_name} "--log_level=message"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM)

  # Add dependencies on shared build and run targets
  add_dependencies(build-tests ${_name})
  add_dependencies(run-tests "run-${_name}")
endmacro()

