cmake_minimum_required(VERSION 3.20)

project(Xake LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)  # mejor para depurar que Release puro
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)

option(XAKE_BUILD_TESTS "Build unit tests" ON)

add_subdirectory(src)

if(XAKE_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
