cmake_minimum_required(VERSION 3.31)
project(http-server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_executable(http-server src/main.cpp
        src/Epoller/Epoll.cpp
        src/threadpool/threadpool.cpp)

target_link_libraries(http-server Threads::Threads)

message(STATUS "Build target:http-server")
message(STATUS "C++ standard:${CMAKE_CXX_STANDARD}")