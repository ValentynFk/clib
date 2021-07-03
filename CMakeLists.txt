cmake_minimum_required(VERSION 3.5)
project(dummy)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wall -g -O3 -Wextra -pedantic -Werror)

find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    set(OPENSSL_USE_STATIC_LIBS TRUE)
endif()

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS random)
if(BOOST_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    link_directories(${Boost_LIBRARY_DIR})
    add_definitions(-DBOOST_ALL_NO_LIB)
    add_definitions(-DBOOST_ALL_DYN_LINK)
endif()

option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
    add_subdirectory(benchmarks)
endif()

#add_subdirectory(contrib/ChaiScript)
message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")

include_directories(SessionManager)
include_directories(SimpleSignal)
include_directories(HPHashMap)
include_directories(${OPENSSL_INCLUDE_DIR})
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries( ${PROJECT_NAME} PRIVATE Boost::random ${OPENSSL_LIBRARIES} OpenSSL::Crypto OpenSSL::SSL)
target_compile_features(${PROJECT_NAME} PRIVATE
    cxx_lambdas
    cxx_inline_namespaces
    cxx_noexcept
    cxx_nullptr
    cxx_range_for
    cxx_return_type_deduction
)


# Create the gcov target. Run coverage tests with 'make gcov'
#add_custom_target(gcov
#    COMMAND ${CMAKE_MAKE_PROGRAM} test
#    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#)
#add_custom_command(TARGET gcov
#    COMMAND echo "=================== GCOV ===================="
#    COMMAND gcov -b "${CMAKE_SOURCE_DIR}/*.cpp" -o ${OBJECT_DIR} > CoverageSummary.tmp
#    COMMAND echo "-- Coverage files have been output to ${CMAKE_BINARY_DIR}/coverage"
#    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage
#)
#add_dependencies(gcov ${PROJECT_NAME})

#set_property(TARGET gcov APPEND_STRING PROPERTY COMPILE_FLAGS " -g -O0 -Wall -fprofile-arcs -ftest-coverage")
#set_property(TARGET gcov APPEND_STRING PROPERTY LINK_FLAGS " -lgcov --coverage")
#set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES coverage)
