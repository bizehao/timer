cmake_minimum_required(VERSION 3.14)
project(timer)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

add_executable(timer test.cpp)

target_link_libraries(timer Threads::Threads)