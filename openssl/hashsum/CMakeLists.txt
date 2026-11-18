cmake_minimum_required(VERSION 4.0)

project(hashsum CXX)

set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

find_package(OpenSSL)

if(NOT OpenSSL_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(OPENSSL REQUIRED openssl)
    set(OPENSSL_INCLUDE_DIR ${OPENSSL_INCLUDE_DIRS})
    set(OPENSSL_LIBRARIES ${OPENSSL_LIBRARIES})
endif()

add_executable(hashsum hashsum.cpp)

target_include_directories(hashsum PRIVATE ${OPENSSL_INCLUDE_DIR})
target_link_libraries(hashsum PRIVATE ${OPENSSL_LIBRARIES})

message(STATUS "Using CXX Compiler: ${CMAKE_CXX_COMPILER}")
