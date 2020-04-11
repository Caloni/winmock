cmake_minimum_required (VERSION 3.13)
project (winmock)

add_subdirectory(client)

option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(unittest)
endif()

