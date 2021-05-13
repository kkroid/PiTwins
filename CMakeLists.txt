cmake_minimum_required(VERSION 3.13)
project(PiTwins)

MESSAGE(STATUS "current platform: ${CMAKE_SYSTEM_NAME}")
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_definitions(-DPi=1)
ENDIF ()

add_definitions(-DOLD_SERVO_DRIVER=1)

find_package(cppzmq)
find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 11)

include_directories(ThirdParty/cptl)
include_directories(ThirdParty/json)
include_directories(ThirdParty/spdlog)
include_directories(ThirdParty/PiServo)
include_directories(Public)
include_directories(${OpenCV_INCLUDE_DIRS})

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_executable(PiTwins PiTwins.cpp
            Private/VideoPublisher.cpp
            Private/StatusPublisher.cpp
            ThirdParty/PiServo/Emakefun_MotorDriver.cpp
            ThirdParty/PiServo/Emakefun_MotorShield.cpp
            ThirdParty/PiServo/Raspi_i2c.cpp
            Public/servo.h)
    find_library(wiringPi_LIB wiringPi)
    target_link_libraries(
            PiTwins
            ${OpenCV_LIBS}
            cppzmq
            ${wiringPi_LIB}
    )
ELSE ()
    add_executable(PiTwins PiTwins.cpp
            Private/VideoPublisher.cpp
            Private/StatusPublisher.cpp)
    target_link_libraries(
            PiTwins
            ${OpenCV_LIBS}
            cppzmq
    )
ENDIF ()
