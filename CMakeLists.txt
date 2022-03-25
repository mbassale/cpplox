cmake_minimum_required(VERSION 3.10)

project(CppLox VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(config.h.in config.h)

file(GLOB SOURCES
    ./*.h
    ./*.cpp
)

add_executable(cpplox ${SOURCES})