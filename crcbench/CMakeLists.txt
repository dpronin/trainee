cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(crcbench VERSION 0.1.0 LANGUAGES CXX)

find_package(Boost CONFIG REQUIRED)
find_package(benchmark CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(xroost REQUIRED)

set(CMAKE_CXX_STANDARD 20)

foreach(bench bench1 bench2)
    add_executable(${bench})
    target_link_libraries(${bench} PRIVATE
        Threads::Threads
        benchmark::benchmark
        benchmark::benchmark_main
        Boost::boost
    )
    target_compile_features(${bench} PRIVATE cxx_std_20)
endforeach()

target_sources(bench1 PRIVATE
    crc32c.h
    crc32c.cpp
    bench1.cpp
)

target_sources(bench2 PRIVATE
    bench2.cpp
)

target_link_libraries(bench2 PRIVATE
    xroost::xroost
)
