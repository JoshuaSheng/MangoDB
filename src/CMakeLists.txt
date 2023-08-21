cmake_minimum_required(VERSION 3.22)
project(db)

set(CMAKE_CXX_STANDARD 17)
add_library(dblib STATIC
        DAL.cpp
        # Add more source files as needed
        meta.cpp meta.h const.h page.cpp page.h)

target_include_directories(dblib PUBLIC DAL.h)