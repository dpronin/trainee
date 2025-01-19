cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(spmcqueue VERSION 0.1.0 LANGUAGES CXX)

find_package(xroost REQUIRED)

add_executable(spmcqueue
    interprocessspmcqueue.cpp
)
target_compile_features(spmcqueue PRIVATE cxx_std_20)
target_link_libraries(spmcqueue PRIVATE xroost::xroost)
