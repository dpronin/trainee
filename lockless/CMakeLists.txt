cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(lockless VERSION 0.1.0 LANGUAGES CXX)

add_executable(interprocessspmcqueue
    interprocessspmcqueue.cpp
)
target_compile_features(interprocessspmcqueue PRIVATE cxx_std_20)
