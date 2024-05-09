cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(hello_world VERSION 0.1.0 LANGUAGES CXX)

add_executable(${PROJECT_NAME}
    hello_world.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
