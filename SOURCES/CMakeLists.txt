cmake_minimum_required(VERSION 3.8)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-Wall -std=c++17 -pedantic ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG -DDEBUG")
  set(CMAKE_CXX_FLAGS_COVERAGE "-g --coverage")
elseif (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /permissive- /D_HAS_TR1_NAMESPACE /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING ")
endif()


include_directories(.)

#add_executable(fixed_vector fixed_vector.cpp fixed_vector.h)

add_executable(fixed_vector_test vector_tests.cpp)
