cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(pybind_test VERSION 0.0.1 LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(${PROJECT_NAME}
    pybind_test.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE
    cxx_std_17
)
