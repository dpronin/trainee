cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(spscqueue VERSION 0.1.0 LANGUAGES CXX)

find_package(xroost REQUIRED)

add_executable(spscqueue
    interprocessspscqueue.cpp
)
target_compile_features(spscqueue PRIVATE cxx_std_20)
target_link_libraries(spscqueue PRIVATE xroost::xroost)
