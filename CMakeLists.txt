cmake_minimum_required(VERSION 3.17)
project(Qucksort)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -pthread)

add_executable(Qucksort main.cpp)