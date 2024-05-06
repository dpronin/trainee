cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(interthreadbinarysemaphore VERSION 0.1.0 LANGUAGES CXX)

add_executable(${PROJECT_NAME}
    ${PROJECT_NAME}.cpp
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
