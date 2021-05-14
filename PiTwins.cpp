#include <future>
#include <iostream>
#include <VideoPublisher.h>
#include <StatusPublisher.h>
#include "json.hpp"
#include "zmq.hpp"

#ifdef Pi
#if OLD_SERVO_DRIVER

#include "servo.h"

#else
#include "MotorShield.h"
#endif

#define PIN_BASE 300
#define MAX_PWM 4096
#define HERTZ 40
#endif


using namespace std;
using namespace nlohmann;


const auto ZMQ_PUB_ADDR = "tcp://*:5555";
const auto ZMQ_PULL_ADDR = "tcp://*:5556";


#ifdef Pi

void CMDReceiverThread(socket_t *receiver) {
#if OLD_SERVO_DRIVER
    Servo *servo = Servo::getInstance();
    wiringPiSetup();
    int fd = servo->pca9685Setup(PIN_BASE, 0x40, HERTZ);
    if (fd < 0) {
        spdlog::error("Error in setup");
        return;
    }

    // Reset all output
    servo->pca9685PWMReset(fd);
    // pwmWrite(PIN_BASE + 16, 400);
#else
    MotorShield Pwm(0x40);
    Pwm.begin(50);
    Servo *myServo1 = Pwm.getServo(1);
    Servo *myServo2 = Pwm.getServo(2);
#endif
    int location = 250;
    int location2 = 200;
    while (true) {
        zmq::message_t msg;
        const auto ret = receiver->recv(msg, recv_flags::none);
        if (!ret) {
            spdlog::error("CMDReceiver received error:{}", ret.value());
            break;
        }
        spdlog::info("CMDReceiver received:{}", msg.to_string());

        json obj = json::parse(msg.to_string());
        int pin = obj["payload"]["pin"];
        int value = obj["payload"]["value"];
        int key = obj["payload"]["key"];
        int interval = 1;
        switch (key) {
            case 0:
                location2 -= interval;
#if OLD_SERVO_DRIVER
                pwmWrite(pin + 1, location2);
#else
                myServo1->writeServo(location2);
#endif
                spdlog::info("pin:{}, location2:{}", pin + 1, location2);
                break;
            case 1:
                location2 += interval;
#if OLD_SERVO_DRIVER
                pwmWrite(pin + 1, location2);
#else
                myServo1->writeServo(location2);
#endif
                spdlog::info("pin:{}, location2:{}", pin + 1, location2);
                break;
            case 2:
                location += interval;
#if OLD_SERVO_DRIVER
                pwmWrite(pin, location);
#else
                myServo2->writeServo(location);
#endif
                break;
            case 3:
                location -= interval;
#if OLD_SERVO_DRIVER
                pwmWrite(pin, location);
#else
                myServo2->writeServo(location);
#endif
                break;
            default:
                break;
        }
    }
    receiver->close();
}

#endif

void VideoPublisherThread(socket_t *publisher, char* id) {
    Publisher *videoPublisher = new VideoPublisher(publisher);
    videoPublisher->start(id);
}

void StatusPublisherThread(socket_t *publisher) {
    Publisher *statusPublisher = new StatusPublisher(publisher);
    statusPublisher->start(nullptr);
}

int main(int argc, char *argv[]) {
    spdlog::info("start zmq server:{}", argv[1]);
    zmq::context_t ctx;
    socket_t *publisher;
    socket_t *receiver;
    publisher = new socket_t(ctx, zmq::socket_type::pub);
    publisher->bind(ZMQ_PUB_ADDR);
    receiver = new socket_t(ctx, zmq::socket_type::pull);
    receiver->bind(ZMQ_PULL_ADDR);

#ifdef Pi
    auto cmdReceiverThread = async(launch::async, CMDReceiverThread, receiver);
#endif

    auto videoPublisherThread = async(launch::async, VideoPublisherThread, publisher, argv[1]);
    auto statusPublisherThread = async(launch::async, StatusPublisherThread, publisher);
    videoPublisherThread.wait();
    statusPublisherThread.wait();
    publisher->close();
#ifdef Pi
    cmdReceiverThread.wait();
#endif
    spdlog::info("thread done");
}
