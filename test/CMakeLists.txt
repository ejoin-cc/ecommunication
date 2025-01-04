cmake_minimum_required(VERSION 3.10)

include(GoogleTest)

file(GLOB_RECURSE ECOMM_TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*_test.cpp")

# #####################################################################################################################
# MAKE TARGETS
# #####################################################################################################################
add_custom_target(check-test COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

# #####################################################################################################################
# MAKE XXX_test
# #####################################################################################################################
foreach (ecomm_test_source ${ECOMM_TEST_SOURCES})
    # Create a human readable name.
    get_filename_component(ecomm_test_filename ${ecomm_test_source} NAME)
    string(REPLACE ".cpp" "" ecomm_test_name ${ecomm_test_filename})

    # Add the test target separately and as part of "make check-test".
    add_executable(${ecomm_test_name} EXCLUDE_FROM_ALL ${ecomm_test_source} "${PROJECT_SOURCE_DIR}/test/backtrace.cpp")
#    add_backward(${ecomm_test_name})
    add_dependencies(check-test ${ecomm_test_name})

    gtest_discover_tests(${ecomm_test_name}
            EXTRA_ARGS
            --gtest_output=xml:${CMAKE_BINARY_DIR}/test/${ecomm_test_name}.xml
            --gtest_catch_exceptions=0
            DISCOVERY_TIMEOUT 120
            PROPERTIES
            TIMEOUT 120
    )

    target_link_libraries(${ecomm_test_name} ecomm gtest gmock_main)

    # Set test target properties and dependencies.
    set_target_properties(${ecomm_test_name}
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
            COMMAND ${ecomm_test_name}
    )
endforeach ()