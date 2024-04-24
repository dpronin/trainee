cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(prefix_tree VERSION 0.1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE
    prefix_tree.hpp
)
target_include_directories(${PROJECT_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

find_package(GTest CONFIG REQUIRED)

add_executable(main
    main.cpp
)
target_compile_features(main PRIVATE cxx_std_23)
target_link_libraries(main PRIVATE
    ${PROJECT_NAME}
    gtest::gtest
)
