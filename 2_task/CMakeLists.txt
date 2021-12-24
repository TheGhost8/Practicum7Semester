cmake_minimum_required(VERSION 3.20)
project(2_task)

set(CMAKE_CXX_STANDARD 17)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(2_task main.cpp AnnealingSimulation.cpp AnnealingSimulation.h Schedule.cpp Schedule.h)

target_link_libraries(2_task Threads::Threads)