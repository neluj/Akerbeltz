cmake_minimum_required(VERSION 3.20)

project(Xake LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Warnings por target (mejor que variables globales)
add_compile_options(-Wall -Wextra -Wpedantic)

# Tipos de build útiles
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)  # mejor para depurar que Release puro
endif()

find_package(Threads REQUIRED)

enable_testing()
add_subdirectory(src)
add_subdirectory(test)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE src Threads::Threads)
target_link_libraries(src PUBLIC Threads::Threads)
target_include_directories(src PUBLIC src)