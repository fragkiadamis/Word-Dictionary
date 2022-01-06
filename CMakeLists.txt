cmake_minimum_required(VERSION 3.20)
project(Assignment_2 C)

set(CMAKE_C_STANDARD 99)

add_executable(Assignment_2 main.c)

target_link_libraries(Assignment_2 PRIVATE m)
