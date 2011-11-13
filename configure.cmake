# Copyright (c) 2011, Christian Rorvik
# Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

vpm_depend(boost)

add_definitions(-DBOOST_TEST_ALTERNATIVE_INIT_API)

vpm_include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

macro(crunch_add_test _name)
  add_custom_target("run-${_name}"
    ALL
    ${_name} "--log_level=message"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM)
endmacro()

