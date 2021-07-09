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
include_directories(ThirdParty/Pi)
include_directories(Public)
include_directories(ThirdParty/libevent/include)
include_directories(ThirdParty/evpp/public)
include_directories(${OpenCV_INCLUDE_DIRS})

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

add_library(zmq SHARED IMPORTED)

add_subdirectory(ThirdParty/evpp)
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_library(wiringPi_LIB wiringPi)

    add_executable(PiTwins PiTwins.cpp
            Private/VideoPublisher.cpp
            Private/StatusPublisher.cpp
            ThirdParty/Pi/MotorDriver.cpp
            ThirdParty/Pi/MotorShield.cpp
            ThirdParty/Pi/RaspiI2c.cpp
            )
    set_target_properties(zmq PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/ThirdParty/zmq/pi/libzmq.so.5.2.3)

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

    set_target_properties(zmq PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/ThirdParty/zmq/mac/libzmq.so.5.2.3)

    target_link_libraries(
            PiTwins
            ${CMAKE_THREAD_LIBS_INIT}
            ${OpenCV_LIBS}
    )
ENDIF ()
