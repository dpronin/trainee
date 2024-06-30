cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(pipeline_model VERSION 0.1.0 LANGUAGES CXX)

find_package(Boost CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    pipeline_model.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Boost::boost
    Boost::system
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
