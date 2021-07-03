add_subdirectory("${PROJECT_SOURCE_DIR}/benchmark" "benchmark")

set(BENCH_SOURCES
        bench.cpp
        )

set(DEPENDENCY_SOURCES
        ${PROJECT_SOURCE_DIR}/SessionManager/SessionManager.hpp
        ${PROJECT_SOURCE_DIR}/SimpleSignal/SimpleSignal.hpp
        )

add_executable(benchmarkall ${BENCH_SOURCES} ${DEPENDENCY_SOURCES})
target_link_libraries(benchmarkall benchmark)
target_include_directories(benchmarkall PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(benchmarkall PRIVATE ${OPENSSL_INCLUDE_DIR})
