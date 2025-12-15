
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
positionTest.cpp
moveTest.cpp
movegenTest.cpp
typesTest.cpp
#evaluateTest.cpp
#uciTest.cpp
pvtableTest.cpp
bitboardsTest.cpp
attacksTest.cpp
#searchTest.cpp
)

target_link_libraries(${PROJECT_NAME}-test PRIVATE GTest::gtest_main xake_core)
target_compile_options(${PROJECT_NAME}-test PRIVATE -Wall -Wextra -Wpedantic $<$<CONFIG:Release>:-O3>)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}-test)
