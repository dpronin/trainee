cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(sort VERSION 1.0.0 LANGUAGES CXX)

add_executable(sort
    sort.cpp
)
target_compile_features(sort PRIVATE cxx_std_23)

add_executable(insertion_sort_one_by_one
    insertion_sort_one_by_one.cpp
)
target_compile_features(insertion_sort_one_by_one PRIVATE cxx_std_23)
