# Copyright (c) 2011, Christian Rorvik
# Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

add_executable(cpuinfo
  main.cpp)

target_link_libraries(cpuinfo
  crunch_concurrency_lib)