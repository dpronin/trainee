cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(cpp20semaphore VERSION 0.1.0 LANGUAGES CXX)

add_executable(cpp20binarysemaphore
    cpp20binarysemaphore.cpp
)

target_compile_features(cpp20binarysemaphore PRIVATE cxx_std_20)
