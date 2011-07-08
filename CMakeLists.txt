# Copyright (c) 2011, Christian Rorvik
# Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

cmake_minimum_required(VERSION 2.8)

#
# Compiler flags
#
if(MSVC)
  add_definitions("-D_WIN32_WINNT=0x0501")
  add_definitions("-Dnoexcept=throw()")

  # Enable parallel compilation
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

  # Set warning level to highest
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

  # Disable buffer security checks
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GS-")
  
  # Enable reasonable inlining in Release With Debug Info
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /Ob2")
  
  # Enable intrinsics
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Oi")
  
  # Enable floating point optimizations (for all configurations)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast") 
  
  # Link time code gen
  # No way to set library linker flags at the moment. Disabling for now.
  # set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL")
  # set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
  
  # Disable some warnings
  # warning C4480: nonstandard extension used: specifying underlying type for enum 'Crunch::CpuidFunction'
  # warning C4481: nonstandard extension used: override specifier 'override'
  # warning C4482: nonstandard extension used: enum 'Crunch::CpuidFunction' used in qualified name
  # warning C4127: conditional expression is constant 
  # (boost utf) warning C4535: calling _set_se_translator() requires /EHa
  # (boost utf) warning C4702: unreachable code
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4480 /wd4481 /wd4482 /wd4127 /wd4535 /wd4702")
else()
  add_definitions("-Dnullptr=__null")
  add_definitions("-Dnoexcept=throw()")
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
find_package(Boost COMPONENTS thread date_time)
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
set(PROJECTS
  crunch_base
  crunch_concurrency
  crunch_concurrency_tasks
  cpuinfo)

foreach(_p ${PROJECTS})
  include_directories("${CMAKE_SOURCE_DIR}/${_p}/include")
  add_subdirectory(${_p})
endforeach()

