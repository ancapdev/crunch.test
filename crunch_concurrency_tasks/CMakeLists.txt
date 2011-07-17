# Copyright (c) 2011, Christian Rorvik
# Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

crunch_add_library(crunch_concurrency_tasks_lib
  include/crunch/concurrency/task.hpp
  source/task.cpp)

target_link_libraries(crunch_concurrency_tasks_lib
  crunch_concurrency_lib)
