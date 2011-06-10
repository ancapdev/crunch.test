if(WIN32)
  set(_platformFiles
    include/crunch/concurrency/platform/win32/atomic.hpp
    include/crunch/concurrency/platform/win32/atomic_ops_x86.hpp
    include/crunch/concurrency/platform/win32/atomic_storage.hpp
    include/crunch/concurrency/platform/win32/atomic_word.hpp)
elif(UNIX)
  set(_platformFiles
    include/crunch/concurrency/platform/linux/atomic.hpp
    include/crunch/concurrency/platform/linux/atomic_ops_x86.hpp
    include/crunch/concurrency/platform/linux/atomic_storage.hpp
    include/crunch/concurrency/platform/linux/atomic_word.hpp)
endif()

add_library(crunch_concurrency_lib
  include/crunch/concurrency/atomic.hpp
  include/crunch/concurrency/lock_free_slist.hpp
  include/crunch/concurrency/event.hpp
  include/crunch/concurrency/fence.hpp
  include/crunch/concurrency/memory_order.hpp
  include/crunch/concurrency/meta_scheduler.hpp
  include/crunch/concurrency/scheduler.hpp
  include/crunch/concurrency/waitable.hpp
  source/event.cpp
  source/meta_scheduler.cpp
  ${_platformFiles})

target_link_libraries(crunch_concurrency_lib
  crunch_base_lib)

add_executable(crunch_concurrency_test
  test/atomic_tests.cpp
  test/event_tests.cpp
  test/main.cpp)

target_link_libraries(crunch_concurrency_test
  crunch_concurrency_lib
  ${Boost_LIBRARIES})

crunch_add_test(crunch_concurrency_test)