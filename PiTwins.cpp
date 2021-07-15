#define USE_ZMQ 0

#if USE_ZMQ
#include <future>
#include <iostream>
#include <VideoPublisher.h>
#include <StatusPublisher.h>
#include "zmq.hpp"

using namespace nlohmann;

#define ZMQ_PUB_ADDR "tcp://*:5555"
#else

#include "Server.h"
#include <tcp_conn.h>

#endif

using namespace std;

int main(int argc, char *argv[]) {
#if USE_ZMQ
    // spdlog::info("start zmq server:{}", argv[1]);
    zmq::context_t ctx;
    socket_t *publisher;
    publisher = new socket_t(ctx, zmq::socket_type::pub);
    publisher->bind(ZMQ_PUB_ADDR);
    auto *cmdReceiver = new socket_t(ctx, zmq::socket_type::sub);
    cmdReceiver->bind(ZMQ_PULL_ADDR);
    cmdReceiver->set(sockopt::subscribe, "");

    auto *videoPublisher = new VideoPublisher(publisher, cmdReceiver);
    auto videoPublisherThread = async(launch::async, [](char* id, Publisher *videoPublisher) {
        videoPublisher->start(id);
    }, argv[1], videoPublisher);
    //
    // auto *statusPublisher = new StatusPublisher(publisher);
    // auto statusPublisherThread = async(launch::async, [](Publisher *statusPublisher) {
    //     statusPublisher->start(nullptr);
    // }, statusPublisher);

    videoPublisherThread.wait();
    // statusPublisherThread.wait();

    delete videoPublisher;
    // delete statusPublisher;


    // cmdReceiver->unbind(ZMQ_PULL_ADDR);
    // cmdReceiver->close();
    // delete cmdReceiver;
    // publisher->unbind(ZMQ_PUB_ADDR);
    // publisher->close();
    // delete publisher;

    spdlog::info("thread done");
#else
    Server::getInstance().init("0.0.0.0:5555", [](const evpp::TCPConnPtr &connPtr) {
        if (connPtr->IsConnected()) {
            spdlog::info("Client {} Connected", connPtr->remote_addr());
            connPtr->Send("Hello:" + connPtr->remote_addr());
        } else {
            spdlog::info("Client {} Disconnected", connPtr->remote_addr());
        }
    }, [](const evpp::TCPConnPtr &connPtr, evpp::Buffer *buffer) {
        spdlog::info("Server Received A Message:[{}]", buffer->ToString());
    });
    Server::getInstance().run();
#endif
}
