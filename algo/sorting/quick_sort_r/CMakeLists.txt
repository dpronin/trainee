cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(quick_sort_r VERSION 0.1.0 LANGUAGES CXX)

add_executable(${PROJECT_NAME}
    quick_sort_r.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
