cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(shmem_chat VERSION 0.1.0 LANGUAGES CXX)

foreach(exe_name client server)
    add_executable(${exe_name}
        ${exe_name}.cpp
    )
    target_compile_features(${exe_name} PRIVATE cxx_std_20)
    target_include_directories(${exe_name} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
endforeach()
