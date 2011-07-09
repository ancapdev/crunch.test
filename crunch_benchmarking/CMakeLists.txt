add_library(crunch_benchmarking_lib
  include/crunch/benchmarking/stopwatch.hpp
  include/crunch/benchmarking/statistical_profiler.hpp
  source/statistical_profiler.cpp)
  
target_link_libraries(crunch_benchmarking_lib
  crunch_base_lib)