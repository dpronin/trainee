cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(sort VERSION 1.0.0 LANGUAGES CXX)

add_executable(${PROJECT_NAME}
    sort.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
