cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(shared_memory VERSION 0.1.0 LANGUAGES CXX)

find_package(xroost REQUIRED)

add_executable(shared_memory
    main.cpp
)
target_compile_features(shared_memory PRIVATE cxx_std_23)
target_link_libraries(shared_memory PRIVATE xroost::xroost)
