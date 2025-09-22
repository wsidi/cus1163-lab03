cmake_minimum_required(VERSION 3.30)
project(week_03 C)

set(CMAKE_C_STANDARD 11)

add_executable(week_03 main.c
        process_manager.h
        process_manager.c)
