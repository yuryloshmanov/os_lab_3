cmake_minimum_required(VERSION 3.17)
project(lab3 C)

set(CMAKE_C_STANDARD 11)
add_compile_options(-g)
add_executable(lab3 main.c polynomial.c polynomial.h vector.c vector.h)
