if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86" OR
    ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
  set(_platformFiles
    include/crunch/concurrency/platform/${PLATFORM_NAME}/atomic.hpp
    include/crunch/concurrency/platform/${PLATFORM_NAME}/atomic_ops_x86.hpp
    include/crunch/concurrency/platform/${PLATFORM_NAME}/atomic_storage.hpp
    include/crunch/concurrency/platform/${PLATFORM_NAME}/atomic_word.hpp)
endif()
    
add_library(crunch_concurrency_lib
  include/crunch/concurrency/atomic.hpp
  include/crunch/concurrency/lock_free_slist.hpp
  include/crunch/concurrency/event.hpp
  include/crunch/concurrency/fence.hpp
  include/crunch/concurrency/future.hpp
  include/crunch/concurrency/lock_guard.hpp
  include/crunch/concurrency/memory_order.hpp
  include/crunch/concurrency/meta_scheduler.hpp
  include/crunch/concurrency/promise.hpp
  include/crunch/concurrency/scheduler.hpp
  include/crunch/concurrency/thread_local.hpp
  include/crunch/concurrency/waitable.hpp
  include/crunch/concurrency/yield.hpp
  include/crunch/concurrency/detail/future_data.hpp
  include/crunch/concurrency/detail/system_event.hpp
  include/crunch/concurrency/detail/system_mutex.hpp
  source/event.cpp
  source/future_data.cpp
  source/meta_scheduler.cpp
  source/processor_affinity.cpp
  source/processor_topology.cpp
  source/platform/${PLATFORM_NAME}/processor_affinity.cpp
  source/platform/${PLATFORM_NAME}/processor_topology.cpp
  source/platform/${PLATFORM_NAME}/system_event.cpp
  source/platform/${PLATFORM_NAME}/system_mutex.cpp
  source/platform/${PLATFORM_NAME}/yield.cpp
  ${_platformFiles})

target_link_libraries(crunch_concurrency_lib
  crunch_base_lib)

if(UNIX)
  target_link_libraries(crunch_concurrency_lib
    pthread)
endif()

add_executable(crunch_concurrency_test
  test/atomic_tests.cpp
  test/event_tests.cpp
  test/future_tests.cpp
  test/main.cpp
  test/meta_scheduler_tests.cpp
  test/processor_topology_tests.cpp)

target_link_libraries(crunch_concurrency_test
  crunch_concurrency_lib)

crunch_add_test(crunch_concurrency_test)