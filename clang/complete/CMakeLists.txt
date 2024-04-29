cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(clang_complete
        VERSION 0.1.0
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)

find_package(Clang REQUIRED CONFIG)

add_executable(${PROJECT_NAME}
    ${PROJECT_NAME}.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CLANG_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE libclang)
