cmake_minimum_required(VERSION 3.8)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-Wall -std=c++17 -pedantic ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG -DDEBUG")
  set(CMAKE_CXX_FLAGS_COVERAGE "-g --coverage")
elseif (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /permissive- /D_HAS_TR1_NAMESPACE /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING ")
  include_directories(.)
  add_subdirectory(gtest)
  include_directories(${CMAKE_SOURCE_DIR}/gtest)
  set(LINK_FLAGS "-pthread ${LINK_FLAGS}")
endif()

add_library(counted counted.h counted.cpp fault_injection.h fault_injection.cpp)

add_executable(fixed_vector fixed_vector.cpp fixed_vector.h)


if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  target_link_libraries(fixed_vector counted -lgtest)
elseif (MSVC)
  target_link_libraries(fixed_vector counted gtest)
endif()

