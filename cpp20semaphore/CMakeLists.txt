cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(cpp20semaphore VERSION 0.1.0 LANGUAGES CXX)

foreach(exe_name cpp20binarysemaphore cpp20countingsemaphore)
    add_executable(${exe_name}
        ${exe_name}.cpp
    )
    target_compile_features(${exe_name} PRIVATE cxx_std_20)
endforeach()
