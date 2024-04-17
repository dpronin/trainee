cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

project(cppmodule VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_library(foo)
target_sources(foo PUBLIC
    FILE_SET
        CXX_MODULES FILES
            foo.cpp
)

add_executable(${PROJECT_NAME}
    cppmodule.cpp
)
target_link_libraries(${PROJECT_NAME} PRIVATE foo)
