cmake_minimum_required(VERSION 3.13)
project(PiTwins)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

set(Pi 1)
add_definitions(-DPi=${Pi})

set(CMAKE_CXX_STANDARD 14)
set(USE_PiRPC_CODE 0)

include_directories(${CMAKE_CURRENT_LIST_DIR}/Public)
include_directories(${CMAKE_CURRENT_LIST_DIR}/ThirdParty/OpenCV/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/ThirdParty/PiRPC/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/ThirdParty/Pi/include)

file(GLOB private_src ${CMAKE_CURRENT_LIST_DIR}/Private/*.cpp)
set(PiRPC_DIR ${CMAKE_CURRENT_LIST_DIR}/ThirdParty/PiRPC)

if (${Pi})
    set(OpenCV_Lib ${CMAKE_CURRENT_LIST_DIR}/ThirdParty/OpenCV/lib/pi/libopencv_world.so.4.5.1)
    set(PiRPC_Lib ${PiRPC_DIR}/lib/pi/libPiRPC.so)
    set(Libevent_Lib ${PiRPC_DIR}/evpp/libevent/lib/pi/libevent-2.1.so.7.0.1)

    find_library(wiringPi_LIB wiringPi)
    file(GLOB pi_src ${CMAKE_CURRENT_LIST_DIR}/ThirdParty/Pi/*.cpp)

    set(PiRPC PiRPC ${wiringPi_LIB})
else ()
    set(OpenCV_Lib ${CMAKE_CURRENT_LIST_DIR}/ThirdParty/OpenCV/lib/macos/libopencv_world.4.5.3.dylib)
    set(PiRPC_Lib ${PiRPC_DIR}/lib/macos/libPiRPC.dylib)
    set(Libevent_Lib ${PiRPC_DIR}/evpp/libevent/lib/macos/libevent-2.1.7.dylib)

    set(PiRPC PiRPC)
endif ()

if (${USE_PiRPC_CODE})
    message("Will use PiRPC Code")
    add_subdirectory(ThirdParty/PiRPC)
else ()
    message("Will use PiRPC Libs")
    add_library(PiRPC SHARED IMPORTED)
    add_library(libevent SHARED IMPORTED)
    set_target_properties(PiRPC PROPERTIES IMPORTED_LOCATION ${PiRPC_Lib})
    set_target_properties(libevent PROPERTIES IMPORTED_LOCATION ${Libevent_Lib})
endif ()

add_library(OpenCV SHARED IMPORTED)
set_target_properties(OpenCV PROPERTIES IMPORTED_LOCATION ${OpenCV_Lib})

add_executable(PiTwins PiTwins.cpp
        ${private_src}
        ${pi_src}
        )
target_link_libraries(
        PiTwins
        OpenCV
        ${PiRPC}
        pthread
)
