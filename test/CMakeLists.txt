
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)
###############

add_executable(${PROJECT_NAME}-test 
types_move_encoding_test.cpp
bitboards_attacks_test.cpp
movegen_perft_test.cpp
position_state_test.cpp
ttable_test.cpp
timemanager_test.cpp
evaluate_test.cpp
search_test.cpp
uci_integration_test.cpp
)

target_link_libraries(${PROJECT_NAME}-test PRIVATE GTest::gtest_main xake_core)
target_compile_options(${PROJECT_NAME}-test PRIVATE -Wall -Wextra -Wpedantic $<$<CONFIG:Release>:-O3>)
set_target_properties(${PROJECT_NAME}-test PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}-test)
