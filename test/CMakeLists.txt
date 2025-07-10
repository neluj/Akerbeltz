
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)
###############

add_executable(${PROJECT_NAME}-test 
positionTest.cpp
moveTest.cpp
#movegenTest.cpps
typesTest.cpp
#perftTest.cpp
evaluateTest.cpp
#uciTest.cpp
#pvtableTest.cpp
bitboardsTest.cpp
attacksTest.cpp
)

target_link_libraries(${PROJECT_NAME}-test  gtest_main src)

file(COPY
perftsuite.epd
DESTINATION ${CMAKE_BINARY_DIR}/test)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}-test)

target_include_directories(${PROJECT_NAME}-test PUBLIC ../src)
