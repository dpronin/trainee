cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(intrusive VERSION 0.1.0 LANGUAGES CXX)

find_package(Boost REQUIRED boost)

add_executable(intrusive
    intrusive.cpp
)
target_compile_features(intrusive PRIVATE cxx_std_20)
target_link_libraries(intrusive PRIVATE boost::boost)
