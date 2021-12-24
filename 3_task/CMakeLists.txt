cmake_minimum_required(VERSION 3.20)
project(3_task)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Google_tests)
add_executable(3_task main.cpp graph.cpp graph.h factory.cpp factory.h)
