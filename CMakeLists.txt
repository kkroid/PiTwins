cmake_minimum_required(VERSION 3.13)
project(PiTwins)

MESSAGE(STATUS "current platform: ${CMAKE_SYSTEM_NAME}")
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_definitions(-DPi=1)
ENDIF ()

set(CMAKE_CXX_STANDARD 14)

include_directories(ThirdParty/Pi)
include_directories(Public)
include_directories(VideoServer/Public)
include_directories(ThirdParty/OpenCV/include)
include_directories(ThirdParty/PiRPC/cptl)
include_directories(ThirdParty/PiRPC/json)
include_directories(ThirdParty/PiRPC/spdlog)
include_directories(ThirdParty/PiRPC/evpp/public)
include_directories(ThirdParty/PiRPC/common/public)
include_directories(ThirdParty/PiRPC/server/public)
include_directories(ThirdParty/PiRPC/client/public)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

add_library(OpenCV SHARED IMPORTED)
set_target_properties(OpenCV PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/ThirdParty/OpenCV/lib/pi/libopencv_world.so.4.5.1)

find_library(wiringPi_LIB wiringPi)

file(GLOB private_src ${CMAKE_CURRENT_LIST_DIR}/Private/*.cpp)
file(GLOB pi_src ${CMAKE_CURRENT_LIST_DIR}/ThirdParty/Pi/*.cpp)
add_executable(PiTwins PiTwins.cpp
        ${private_src}
        ${pi_src}
        )

add_definitions(-DPiRPC_TYPE=0)
add_subdirectory(ThirdParty/PiRPC)

target_link_libraries(
        PiTwins
        zmq
        ${wiringPi_LIB}
        OpenCV
        PiRPC
)
