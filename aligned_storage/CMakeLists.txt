cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(aligned_storage VERSION 0.1.0 LANGUAGES CXX)

add_executable(${PROJECT_NAME}
    aligned_storage.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
