cmake_minimum_required(VERSION 4.2)
project(ProiectSDA)

set(CMAKE_CXX_STANDARD 20)

add_executable(ProiectSDA main.cpp)

target_compile_definitions(ProiectSDA PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")
