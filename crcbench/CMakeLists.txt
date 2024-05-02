cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(crcbench VERSION 0.1.0 LANGUAGES CXX)

find_package(Boost CONFIG REQUIRED)
find_package(benchmark CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    crc32c.h
    crc32c.cpp
    bench.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Boost::boost
    benchmark::benchmark
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
