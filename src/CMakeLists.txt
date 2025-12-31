configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/engine_info.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/engine_info.h
  @ONLY
)

add_library(akerbeltz_core
  position.cpp
  attacks.cpp
  movegen.cpp
  timemanager.cpp
  search.cpp
  evaluate.cpp
  uci.cpp
  ttable.cpp
)

target_include_directories(akerbeltz_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(akerbeltz_core PUBLIC Threads::Threads)

target_compile_options(akerbeltz_core PRIVATE -Wall -Wextra -Wpedantic $<$<CONFIG:Release>:-O3>)

add_executable(Akerbeltz main.cpp)
target_link_libraries(Akerbeltz PRIVATE akerbeltz_core)
target_compile_options(Akerbeltz PRIVATE -Wall -Wextra -Wpedantic $<$<CONFIG:Release>:-O3>)
set_target_properties(Akerbeltz PROPERTIES OUTPUT_NAME "Akerbeltz-${AKERBELTZ_ENGINE_VERSION}")

if(AKERBELTZ_ARCH)
  target_compile_options(akerbeltz_core PRIVATE "-march=${AKERBELTZ_ARCH}")
  target_compile_options(Akerbeltz PRIVATE "-march=${AKERBELTZ_ARCH}")
endif()

set_target_properties(Akerbeltz PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
set_target_properties(akerbeltz_core PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
