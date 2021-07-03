add_subdirectory("${PROJECT_SOURCE_DIR}/googletest" "googletest")

mark_as_advanced(
    BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
    gmock_build_tests gtest_build_samples gtest_build_tests
    gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)

set_target_properties(gtest PROPERTIES FOLDER extern)
set_target_properties(gtest_main PROPERTIES FOLDER extern)
set_target_properties(gmock PROPERTIES FOLDER extern)
set_target_properties(gmock_main PROPERTIES FOLDER extern)

macro(package_add_test TESTNAME)
    add_executable(${TESTNAME} ${ARGN})
    target_link_libraries(${TESTNAME} gtest gmock gtest_main)
    gtest_discover_tests(${TESTNAME}
        WORKING_DIRECTORY ${PROJECT_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
    )
    set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
endmacro()

macro(package_add_test_with_libraries TESTNAME FILES LIBRARIES TEST_WORKING_DIRECTORY)
    add_executable(${TESTNAME} ${FILES})
    target_link_libraries(${TESTNAME} gtest gmock gtest_main ${LIBRARIES})
    gtest_discover_tests(${TESTNAME}
        WORKING_DIRECTORY ${TEST_WORKING_DIRECTORY}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${TEST_WORKING_DIRECTORY}"
    )
    set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
endmacro()

set(TEST_SOURCES
        runtests.cpp
        tSimpleSignal.cpp
        tSessionManager.cpp
)

set(DEPENDENCY_SOURCES
        ${PROJECT_SOURCE_DIR}/SessionManager/SessionManager.hpp
        ${PROJECT_SOURCE_DIR}/SimpleSignal/SimpleSignal.hpp
)

package_add_test(testall ${TEST_SOURCES} ${DEPENDENCY_SOURCES})
target_include_directories(testall PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(testall PRIVATE ${OPENSSL_INCLUDE_DIR})
#package_add_test(tsessionmanager tSessionManager.cpp ${SESSION_MANAGER_SOURCES})
#target_include_directories(tsessionmanager PRIVATE ${PROJECT_SOURCE_DIR}/SessionManager)
#package_add_test(tsimplesignal tSimpleSignal.cpp ${SIMPLE_SIGNAL_SOURCES})
#target_include_directories(tsimplesignal PRIVATE ${PROJECT_SOURCE_DIR}/SimpleSignal)

#package_add_test_with_libraries(test1 test1.cpp lib_to_test "${PROJECT_DIR}/european-test-data/")
