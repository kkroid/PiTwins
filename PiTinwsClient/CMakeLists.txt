cmake_minimum_required(VERSION 3.15)
project(PiTwinsClient)

# for windows
set(OpenCV_DIR ../ThirdParty/OpenCV/win64/x64/vc15/lib)
find_package(OpenCV REQUIRED)
set(ZMQ_DIR ../ThirdParty/zmq/win64)
link_directories(ZMQ_DIR)

set(CMAKE_CXX_STANDARD 14)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(../ThirdParty/zmq/include)
include_directories(../ThirdParty/spdlog)
include_directories(public)

add_executable(PiTwinsClient
        main.cpp
        public/Snowflak.h)

add_library(libzmq STATIC ${ZMQ_DIR}/libzmq-v142-mt-sgd-4_3_4.lib)

target_link_libraries(
        PiTwinsClient
        ${OpenCV_LIBS}
        libzmq
)
