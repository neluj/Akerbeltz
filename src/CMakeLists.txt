add_library(xake_core
  position.cpp
  attacks.cpp
  movegen.cpp
  timemanager.cpp
  search.cpp
  evaluate.cpp
  uci.cpp
  ttable.cpp
)

target_include_directories(xake_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(xake_core PUBLIC Threads::Threads)

target_compile_options(xake_core PRIVATE -Wall -Wextra -Wpedantic $<$<CONFIG:Release>:-O3>)

add_executable(Xake main.cpp)
target_link_libraries(Xake PRIVATE xake_core)
target_compile_options(Xake PRIVATE -Wall -Wextra -Wpedantic $<$<CONFIG:Release>:-O3>)

set_target_properties(Xake PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
set_target_properties(xake_core PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
