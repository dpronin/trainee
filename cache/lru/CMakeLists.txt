cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(lru VERSION 0.1.0 LANGUAGES CXX)

add_library(flat_lru INTERFACE
    flat_lru.hpp
)
add_library(lru::cache ALIAS flat_lru)

find_package(GTest CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    lru.cpp
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_link_libraries(${PROJECT_NAME} PRIVATE
    lru::cache
    gtest::gtest
)
