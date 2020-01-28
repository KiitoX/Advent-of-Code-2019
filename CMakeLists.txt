cmake_minimum_required(VERSION 3.15)
project(advent2019 C)

set(CMAKE_C_STANDARD 11)

add_executable(advent2019 main.c main.h gpio.c misc.c day1.c day2.c day3.c day4.c intcode.c day5.c day6.c day7.c day8.c day9.c day10.c)

target_link_libraries(advent2019 m) # link math library
