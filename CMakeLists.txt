cmake_minimum_required (VERSION 3.20)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(Xake)

enable_testing()

add_subdirectory(src)
add_subdirectory(test)

add_executable(${PROJECT_NAME} 
main.cpp)

target_link_libraries( ${PROJECT_NAME} PUBLIC src)

target_include_directories(src PUBLIC src)