cmake_minimum_required(VERSION 2.8)

#
# Compiler flags
#
if(MSVC)
  add_definitions("-D_WIN32_WINNT=0x0501")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
else()
  add_definitions("-Dnullptr=NULL")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    # Enable cmpxchg16
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcx16")
  endif()
endif()

#
# Platform properties
#
if(WIN32)
  set(PLATFORM_NAME "win32")
elseif(UNIX)
  set(PLATFORM_NAME "linux")
else()
  message(FATAL_ERROR "Unknown platform")
endif()

#
# boost package
#
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
# Work around issue with simultaneous 32 and 64 bit boost installs
if(WIN32)
  set(_root "$ENV{BOOST_ROOT}")
  if(_root STREQUAL "")
    set(BOOST_ROOT "c:/boost/boost_1_46_1")
    set(_root "C:/boost/boost_1_46_1")
  endif()
  set(BOOST_INCLUDEDIR "${_root}/boost")
  if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set(BOOST_LIBRARYDIR "${_root}/lib64")
  else()
    set(BOOST_LIBRARYDIR "${_root}/lib32")
  endif()
endif()
find_package(Boost COMPONENTS system thread date_time)
if(NOT Boost_FOUND)
  message(FATAL_ERROR "boost not found")
endif()
include_directories(${Boost_INCLUDE_DIRS})

#
# boost test
#
add_definitions(-DBOOST_TEST_ALTERNATIVE_INIT_API)

macro(crunch_add_test _name)
  add_custom_target("test-${_name}"
    ALL
    ${_name} "--log_level=message"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM)
endmacro()

#
# libraries and executables
#
include_directories("${CMAKE_SOURCE_DIR}/crunch_base/include")
include_directories("${CMAKE_SOURCE_DIR}/crunch_concurrency/include")
add_subdirectory(crunch_base)
add_subdirectory(crunch_concurrency)

