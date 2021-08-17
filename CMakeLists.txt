cmake_minimum_required(VERSION 3.13)
project(PiTwins)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

set(Pi 1)
add_definitions(-DPiRPC_TYPE=0)
add_definitions(-DPi=${Pi})

set(CMAKE_CXX_STANDARD 14)
set(USE_PiRPC_CODE 0)

include_directories(Public)
include_directories(ThirdParty/OpenCV/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/ThirdParty/PiRPC/include)

add_library(OpenCV SHARED IMPORTED)
if (${Pi})
    include_directories(ThirdParty/Pi)
    set_target_properties(OpenCV PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/ThirdParty/OpenCV/lib/pi/libopencv_world.so.4.5.1)
    find_library(wiringPi_LIB wiringPi)
    file(GLOB pi_src ${CMAKE_CURRENT_LIST_DIR}/ThirdParty/Pi/*.cpp)
else()
    set_target_properties(OpenCV PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/ThirdParty/OpenCV/lib/macos/libopencv_world.4.5.2.dylib)
endif ()

file(GLOB private_src ${CMAKE_CURRENT_LIST_DIR}/Private/*.cpp)

if (${USE_PiRPC_CODE})
    add_subdirectory(ThirdParty/PiRPC)
    set(PiRPC PiRPC)
else()
    add_library(PiRPC SHARED IMPORTED)
    add_library(evpp SHARED IMPORTED)
    add_library(libevent SHARED IMPORTED)
    set(PiRPC_DIR ${CMAKE_CURRENT_LIST_DIR}/ThirdParty/PiRPC)
    set_target_properties(PiRPC PROPERTIES IMPORTED_LOCATION ${PiRPC_DIR}/lib/pi/libPiRPC.so)
    set_target_properties(evpp PROPERTIES IMPORTED_LOCATION ${PiRPC_DIR}/evpp/lib/pi/libevpp.so)
    set_target_properties(libevent PROPERTIES IMPORTED_LOCATION ${PiRPC_DIR}/evpp/libevent/lib/pi/libevent-2.1.so.7.0.1)
    set(PiRPC PiRPC evpp libevent)
endif()

if (${Pi})
    add_executable(PiTwins PiTwins.cpp
            ${private_src}
            ${pi_src}
            )
    target_link_libraries(
            PiTwins
            ${wiringPi_LIB}
            OpenCV
            ${PiRPC}
            pthread
    )
else ()
    add_executable(PiTwins PiTwins.cpp
            ${private_src}
            )
    target_link_libraries(
            PiTwins
            OpenCV
            ${PiRPC}
    )
endif ()
