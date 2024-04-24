cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(cache_lru VERSION 0.1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE
    flat_lru.hpp
    generic_lru.hpp
)
target_include_directories(${PROJECT_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

add_library(cache::lru ALIAS cache_lru)

find_package(GTest CONFIG REQUIRED)

add_executable(ut
    ut.cpp
)
target_compile_features(ut PRIVATE cxx_std_23)
target_link_libraries(ut PRIVATE
    cache::lru
    gtest::gtest
)
