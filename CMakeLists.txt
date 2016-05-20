cmake_minimum_required(VERSION 3.5)
project(JoyDriver2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

link_libraries(wiringPi)

set(SOURCE_FILES main.cpp)
add_executable(JoyDriver2 ${SOURCE_FILES})