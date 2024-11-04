cmake_minimum_required(VERSION 3.21)

project(wiringPi LANGUAGES CXX)

add_library(wiringPi STATIC
    src/wiringSerial.cpp
)

target_include_directories(wiringPi PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
