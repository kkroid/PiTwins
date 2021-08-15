cmake_minimum_required(VERSION 3.13)
project(PiTwins)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

set(Pi 1)
add_definitions(-DPiRPC_TYPE=0)
add_definitions(-DPi=${Pi})

set(CMAKE_CXX_STANDARD 14)

include_directories(Public)
include_directories(ThirdParty/OpenCV/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/ThirdParty/PiRPC/include)
#include_directories(ThirdParty/PiRPC/cptl)
#include_directories(ThirdParty/PiRPC/json)
#include_directories(ThirdParty/PiRPC/spdlog)
#include_directories(ThirdParty/PiRPC/evpp/public)
#include_directories(ThirdParty/PiRPC/common/public)
#include_directories(ThirdParty/PiRPC/server/public)
#include_directories(ThirdParty/PiRPC/client/public)

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

add_subdirectory(ThirdParty/PiRPC)

if (${Pi})
    add_executable(PiTwins PiTwins.cpp
            ${private_src}
            ${pi_src}
            )
    target_link_libraries(
            PiTwins
            ${wiringPi_LIB}
            OpenCV
            PiRPC
    )
else ()
    add_executable(PiTwins PiTwins.cpp
            ${private_src}
            )
    target_link_libraries(
            PiTwins
            OpenCV
            PiRPC
    )
endif ()
