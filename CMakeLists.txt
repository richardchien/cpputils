cmake_minimum_required(VERSION 3.9)
project(cpputils)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
add_executable(test test.cpp)
