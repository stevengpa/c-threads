cmake_minimum_required(VERSION 4.0)
project(c_threads C)

set(CMAKE_C_STANDARD 23)

include_directories(inc/)

add_executable(c_threads main.c
        src/threadpool.c)
