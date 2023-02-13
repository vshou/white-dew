cmake_minimum_required(VERSION 3.24)

project(white_dew C)

set(CMAKE_C_STANDARD 11)

include_directories(headers)

add_library(white_dew STATIC memory_pool.c)
