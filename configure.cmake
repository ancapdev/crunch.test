# Copyright (c) 2011-2013, Christian Rorvik
# Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

vpm_set_default_variant(boost proxy)
vpm_set_default_version(boost 1.53.0)
vpm_depend(boost)

vpm_include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

set(CRUNCH_TEST_DIR ${CMAKE_CURRENT_LIST_DIR})

if(NOT Boost_USE_STATIC_LIBS)
  add_definitions("-DBOOST_TEST_DYN_LINK")
endif()

macro(crunch_add_test_executable _name)
  # Add test executable, with main function defined by the boost unit testing framework
  add_executable(${_name}
    ${ARGN}
    ${CRUNCH_TEST_DIR}/source/main.cpp)

  target_link_libraries(${_name}
    boost_unit_test_framework)
endmacro()

macro(crunch_add_test_targets _name)
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

macro(crunch_add_test _name)
  crunch_add_test_executable(${_name} ${ARGN})
  crunch_add_test_targets(${_name})
endmacro()

