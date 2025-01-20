cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(partition VERSION 0.1.0 LANGUAGES CXX)

find_package(xroost REQUIRED)

add_executable(${PROJECT_NAME}
    partition.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_link_libraries(${PROJECT_NAME} PRIVATE xroost::xroost)
