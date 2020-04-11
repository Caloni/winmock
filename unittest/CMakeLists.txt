add_subdirectory("${PROJECT_SOURCE_DIR}/submodules/googletest" "submodules/googletest")

macro(package_add_test TESTNAME)
    # create an executable in which the tests will be stored
    add_executable(${TESTNAME} ${ARGN})
    # link the Google test infrastructure, mocking library, and a default main fuction to
    # the test executable.  Remove g_test_main if writing your own main function.
    target_link_libraries(${TESTNAME} gtest gmock gtest_main ${TESTNAME}_lib)
    # gtest_discover_tests replaces gtest_add_tests,
    # see https://cmake.org/cmake/help/v3.10/module/GoogleTest.html for more options to pass to it
    gtest_discover_tests(${TESTNAME}
        # set a working directory so your project root so that you can find test data via paths relative to the project root
        WORKING_DIRECTORY ${PROJECT_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
    )
    set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
endmacro()

#comment line bellow to disable mock
add_definitions(-DINCL_WINSOCK_API_PROTOTYPES=0)
package_add_test(client_test client_unittest.cpp)
add_library(client_test_lib STATIC client_mock.c ../client/client.c)
target_include_directories(client_test PRIVATE)
