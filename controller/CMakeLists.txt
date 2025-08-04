cmake_minimum_required(VERSION 3.10)
project(HelloCpp)

set(CMAKE_CXX_STANDARD 17)

add_executable(hello src/main.cpp src/arduino.cpp src/scheduler.cpp)