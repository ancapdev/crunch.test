add_library(crunch_base_lib
  include/crunch/base/align.hpp
  include/crunch/base/assert.hpp
  include/crunch/base/debug.hpp
  include/crunch/base/high_frequency_timer.hpp
  include/crunch/base/intrusive_ptr.hpp
  include/crunch/base/memory.hpp
  include/crunch/base/override.hpp
  include/crunch/base/platform.hpp
  include/crunch/base/stack_alloc.hpp
  include/crunch/base/stdint.hpp
  source/assert.cpp
  source/platform/${PLATFORM_NAME}/assert.cpp
  source/platform/${PLATFORM_NAME}/high_frequency_timer.cpp)


add_executable(crunch_base_test
  test/main.cpp
  test/memory_tests.cpp)

target_link_libraries(crunch_base_test
  crunch_base_lib)

crunch_add_test(crunch_base_test)
