cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

project(sort VERSION 1.1.0 LANGUAGES CXX)

find_package(xroost REQUIRED)

add_executable(sort
    sort.cpp
)
target_compile_features(sort PRIVATE cxx_std_23)
target_link_libraries(sort PRIVATE xroost::xroost)

add_executable(insertion_sort_one_by_one
    insertion_sort_one_by_one.cpp
)
target_compile_features(insertion_sort_one_by_one PRIVATE cxx_std_23)
