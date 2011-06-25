if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86" OR
    ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
  set(_platformFiles
    include/crunch/base/arch/x86/cpuid.hpp
    source/arch/x86/cpuid.cpp)
endif()

if(WIN32)
  set(_platformFiles
    ${_platformFiles}
    include/crunch/base/platform/win32/wintypes.hpp)
endif()

add_library(crunch_base_lib
  include/crunch/base/align.hpp
  include/crunch/base/assert.hpp
  include/crunch/base/bit_utility.hpp
  include/crunch/base/debug.hpp
  include/crunch/base/enum_class.hpp
  include/crunch/base/high_frequency_timer.hpp
  include/crunch/base/intrusive_ptr.hpp
  include/crunch/base/memory.hpp
  include/crunch/base/noncopyable.hpp
  include/crunch/base/override.hpp
  include/crunch/base/platform.hpp
  include/crunch/base/stack_alloc.hpp
  include/crunch/base/stream_as_binary.hpp
  include/crunch/base/stdint.hpp
  source/assert.cpp
  source/platform/${PLATFORM_NAME}/assert.cpp
  source/platform/${PLATFORM_NAME}/high_frequency_timer.cpp
  ${_platformFiles})

add_executable(crunch_base_test
  test/intrusive_ptr_tests.cpp
  test/main.cpp
  test/memory_tests.cpp)

target_link_libraries(crunch_base_test
  crunch_base_lib)

crunch_add_test(crunch_base_test)
