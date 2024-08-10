cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(tls VERSION 0.1.0 LANGUAGES CXX)

find_package(OpenSSL REQUIRED)

foreach(app client server)
    add_executable(${app}
        ${app}.cpp
    )
    target_compile_features(${app} PRIVATE cxx_std_23)
    target_link_libraries(${app} PRIVATE openssl::openssl)
endforeach()

