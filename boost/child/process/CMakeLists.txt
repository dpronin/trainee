cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(process VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")

add_subdirectory(src)
