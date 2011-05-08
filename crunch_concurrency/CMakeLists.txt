add_library(crunch_concurrency_lib
  include/crunch/concurrency/event.hpp
  include/crunch/concurrency/meta_scheduler.hpp
  include/crunch/concurrency/scheduler.hpp
  include/crunch/concurrency/waitable.hpp
  source/event.cpp
  source/meta_scheduler.cpp)

target_link_libraries(crunch_concurrency_lib
  crunch_base_lib)

add_executable(crunch_concurrency_test
  test/event_tests.cpp
  test/main.cpp)

target_link_libraries(crunch_concurrency_test
  crunch_concurrency_lib
  ${Boost_LIBRARIES})

crunch_add_test(crunch_concurrency_test)