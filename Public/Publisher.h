//
// Created by Will Zhang on 2021/1/10.
//

#ifndef PITWINS_PUBLISHER_H
#define PITWINS_PUBLISHER_H

#include <zmq.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "spdlog/spdlog.h"
#include "ctpl_stl.h"

using namespace zmq;
using namespace std;
using namespace cv;


class Publisher {
public:

    socket_t *publisher;
    ctpl::thread_pool *threadPool;

    explicit Publisher(socket_t *publisher) : publisher(publisher) {
        threadPool = new ctpl::thread_pool(2);
    }

    virtual ~Publisher() {
        delete publisher;
        delete threadPool;
    };

    virtual void start() = 0;

    virtual void publish(const_buffer buffer, send_flags flags) {
        // threadPool->push([this, buffer, flags](int id) {
        // });
        size_t size = buffer.size();
        // TODO 解决发送时间过长的问题
        publisher->send(buffer, flags);
        spdlog::info("Send a frame, size:{}", size);
    };
};

#endif //PITWINS_PUBLISHER_H