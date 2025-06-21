cmake_minimum_required(VERSION 3.10)
project(mytop)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(mytop ${SOURCES})
