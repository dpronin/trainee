cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(dlopen VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)

foreach(i 1 2)
    add_library(foo.${i} SHARED
        foo.h
        foo.${i}.cpp
    )
    target_compile_features(foo.${i} PRIVATE cxx_std_11)
endforeach()

add_executable(${PROJECT_NAME}
    foo.h
    dlopen.c
)
target_compile_features(${PROJECT_NAME} PRIVATE c_std_99)
