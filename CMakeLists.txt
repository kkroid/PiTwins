cmake_minimum_required(VERSION 3.13)
project(PiTwins)

MESSAGE(STATUS "current platform: ${CMAKE_SYSTEM_NAME}")
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_definitions(-DPi=1)
ENDIF ()

#find_package(cppzmq)
find_package(Threads)
find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 11)

include_directories(ThirdParty/zmq/include)
include_directories(ThirdParty/cptl)
include_directories(ThirdParty/json)
include_directories(ThirdParty/spdlog)
include_directories(ThirdParty/PiServo)
include_directories(Public)
include_directories(${OpenCV_INCLUDE_DIRS})

add_library(zmq STATIC IMPORTED)

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_library(wiringPi_LIB wiringPi)

    add_executable(PiTwins PiTwins.cpp
            Private/VideoPublisher.cpp
            Private/StatusPublisher.cpp
            ThirdParty/PiServo/MotorDriver.cpp
            ThirdParty/PiServo/MotorShield.cpp
            ThirdParty/PiServo/RaspiI2c.cpp
            Public/servo.h)
    set_target_properties(zmq PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/ThirdParty/zmq/pi/libzmq.a)

    target_link_libraries(
            PiTwins
            ${CMAKE_THREAD_LIBS_INIT}
            zmq
            ${OpenCV_LIBS}
            ${wiringPi_LIB}
    )
ELSE ()
    add_executable(PiTwins PiTwins.cpp
            Private/VideoPublisher.cpp
            Private/StatusPublisher.cpp)

    set_target_properties(zmq PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/ThirdParty/zmq/mac/libzmq.a)

    target_link_libraries(
            PiTwins
            ${CMAKE_THREAD_LIBS_INIT}
            ${OpenCV_LIBS}
    )
ENDIF ()
