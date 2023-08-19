cmake_minimum_required(VERSION 3.22)
project(NotMySQL)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(tests)
add_executable(NotMySQL main.cpp DAL.cpp DAL.h)