cmake_minimum_required(VERSION 3.13)
project(PiTwins)

find_package(cppzmq)
find_package(OpenCV REQUIRED)
find_package(spdlog REQUIRED)

set(CMAKE_CXX_STANDARD 11)

include_directories(ThirdParty/cptl)
include_directories(ThirdParty/json)
include_directories(Public)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(PiTwins PiTwins.cpp
        Private/VideoPublisher.cpp
        Private/StatusPublisher.cpp
        Public/servo.h)

find_library(wiringPi_LIB wiringPi)
target_link_libraries(
        PiTwins
        ${OpenCV_LIBS}
        cppzmq
        ${wiringPi_LIB}
        spdlog
)
