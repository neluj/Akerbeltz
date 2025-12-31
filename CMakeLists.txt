cmake_minimum_required(VERSION 3.20)

project(Akerbeltz VERSION 1.0.0 LANGUAGES CXX)

set(AKERBELTZ_ENGINE_VERSION "${PROJECT_VERSION}" CACHE STRING "Engine version string")
set(AKERBELTZ_ENGINE_AUTHOR "Julen Aristondo")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)  # por defecto Release; para depurar especifica Debug o RelWithDebInfo
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR})

find_package(Threads REQUIRED)

option(AKERBELTZ_BUILD_TESTS "Build unit tests" OFF)
set(AKERBELTZ_ARCH "" CACHE STRING "Optional -march value (e.g., native, x86-64-v3). Leave empty for generic builds.")

add_subdirectory(src)

if(AKERBELTZ_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
