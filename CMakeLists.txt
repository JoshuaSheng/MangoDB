cmake_minimum_required(VERSION 3.22)
project(NotMySQL)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
add_subdirectory(tests)
add_executable(MainProject main.cpp)
target_link_libraries(MainProject PRIVATE dblib)