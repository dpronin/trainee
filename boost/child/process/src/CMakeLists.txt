find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(Threads)

add_executable(hello main.cpp)
add_executable(program program.cpp)
target_link_libraries(hello PRIVATE Boost::system)
target_link_libraries(hello PRIVATE Boost::filesystem)
target_link_libraries(hello PRIVATE ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(program PRIVATE ${CMAKE_THREAD_LIBS_INIT})
