add_subdirectory("${PROJECT_SOURCE_DIR}/benchmark" "benchmark")

set(BENCH_SOURCES
        bench.cpp
        )

set(DEPENDENCY_SOURCES
        ${PROJECT_SOURCE_DIR}/SessionManager/SessionManager.hpp
        ${PROJECT_SOURCE_DIR}/SimpleSignal/SimpleSignal.hpp
        )

add_executable(benchall ${BENCH_SOURCES} ${DEPENDENCY_SOURCES})
target_link_libraries(benchall benchmark)
set_target_properties(benchall PROPERTIES FOLDER tests)
target_include_directories(benchall PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(benchall PRIVATE ${OPENSSL_INCLUDE_DIR})
