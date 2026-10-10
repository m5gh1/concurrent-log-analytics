cmake_minimum_required(VERSION 3.16)
project(ConcurrentLogAnalytics VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_library(log_core
    src/LogEntry.cpp
    src/LogParser.cpp
    src/LogFilter.cpp
    src/LogAnalytics.cpp
    src/AlertManager.cpp
    src/Config.cpp
    src/LogPipeline.cpp
)
target_include_directories(log_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(log_core PUBLIC Threads::Threads)
target_compile_options(log_core PRIVATE -Wall -Wextra -Wpedantic)

add_executable(log_analytics src/main.cpp)
target_link_libraries(log_analytics PRIVATE log_core)

enable_testing()
foreach(test_name parser queue analytics alerts)
    add_executable(test_${test_name} tests/test_${test_name}.cpp)
    target_link_libraries(test_${test_name} PRIVATE log_core)
    add_test(NAME ${test_name}_tests COMMAND test_${test_name})
endforeach()

add_executable(container_benchmark benchmarks/benchmark.cpp)
target_compile_options(container_benchmark PRIVATE -Wall -Wextra -Wpedantic)
