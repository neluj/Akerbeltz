
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        797b0ad2a3a45608ecf5c67e6e289d377a3521ca #GTest version tag here
)
FetchContent_MakeAvailable(googletest)
###############

add_executable(${PROJECT_NAME}-test 
positionTest.cpp
moveTest.cpp
movegenTest.cpp
typesTest.cpp
perftTest.cpp
)

target_link_libraries(${PROJECT_NAME}-test  gtest_main src)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}-test)

target_include_directories(${PROJECT_NAME}-test PUBLIC ../src)
