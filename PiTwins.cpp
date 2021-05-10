#include <future>
#include <iostream>
#include <VideoPublisher.h>
#include <StatusPublisher.h>
#include "json.hpp"
#include "zmq.hpp"
#ifdef Pi
#include "servo.h"

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
    Servo *servo = Servo::getInstance();
    wiringPiSetup();
    int fd = servo->pca9685Setup(PIN_BASE, 0x40, HERTZ);
    if (fd < 0) {
        cout << "Error in setup" << endl;
        return;
    }
    // Reset all output
    servo->pca9685PWMReset(fd);
    // pwmWrite(PIN_BASE + 16, 400);
    int location = 250;
    int location2 = 200;
    while (true) {
        zmq::message_t msg;
        const auto ret = receiver->recv(msg, recv_flags::none);
        if (!ret) {
            cout << "CMDReceiver received error:" << ret.value() << endl;
            break;
        }
        cout << "CMDReceiver received:" << msg.to_string() << endl;

        json obj = json::parse(msg.to_string());
        int pin = obj["payload"]["pin"];
        int value = obj["payload"]["value"];
        int key = obj["payload"]["key"];
        int interval = 1;
        switch (key) {
            case 0:
                location2 -= interval;
                pwmWrite(pin + 1, location2);
                break;
            case 1:
                location2 += interval;
                pwmWrite(pin + 1, location2);
                break;
            case 2:
                location += interval;
                pwmWrite(pin, location);
                break;
            case 3:
                location -= interval;
                pwmWrite(pin, location);
                break;
            default:
                break;
        }
    }
    receiver->close();
}
#endif

void VideoPublisherThread(socket_t *publisher) {
    Publisher *videoPublisher = new VideoPublisher(publisher);
    videoPublisher->start();
}

void StatusPublisherThread(socket_t *publisher) {
    Publisher *statusPublisher = new StatusPublisher(publisher);
    statusPublisher->start();
}

int main() {
    cout << "start zmq server" << endl;
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

    auto videoPublisherThread = async(launch::async, VideoPublisherThread, publisher);
    auto statusPublisherThread = async(launch::async, StatusPublisherThread, publisher);
    videoPublisherThread.wait();
    statusPublisherThread.wait();
    publisher->close();
#ifdef Pi
    cmdReceiverThread.wait();
#endif

    cout << "thread done" << endl;
}
