//
// Created by Will Zhang on 2021/7/10.
//

#ifndef PITWINS_SERVER_H
#define PITWINS_SERVER_H

#include <spdlog/spdlog.h>
#include <tcp_server.h>
#include <buffer.h>
#include <tcp_conn.h>
#include <event_loop.h>
#include "slice.h"
#include "buffer.h"


class Server {
private:
    int DEFAULT_THREAD_NUM = 4;
    evpp::TCPServer *server = nullptr;
    evpp::EventLoop *loop = nullptr;
public:
    std::shared_ptr<evpp::TCPConn> tcpConnPtr = nullptr;

    Server() = default;

    ~Server() {
        spdlog::info("~Server");
        delete loop;
        delete server;
    }

    static Server &getInstance() {
        static Server instance;
        return instance;
    }

    void init(const std::string &addr, const evpp::ConnectionCallback &ccb, const evpp::MessageCallback &mcb) {
        loop = new evpp::EventLoop();
        server = new evpp::TCPServer(loop, addr, "PiServer", DEFAULT_THREAD_NUM);
        server->SetConnectionCallback([this, ccb](const evpp::TCPConnPtr &connPtr) {
            if (connPtr->IsConnected()) {
                if (tcpConnPtr == nullptr) {
                    tcpConnPtr = connPtr;
                }
            } else if (connPtr->IsDisconnected()) {
                tcpConnPtr = nullptr;
            }
            ccb(connPtr);
        });
        server->SetMessageCallback([this, mcb](const evpp::TCPConnPtr &connPtr, evpp::Buffer *buffer) {
            mcb(connPtr, buffer);
        });
    }

    void run();

    void send(const char *s) {
        send(s, strlen(s));
    }

    void send(const void *d, size_t dlen) {
        if (tcpConnPtr) {
            tcpConnPtr->Send(d, dlen);
        }
    }

    void send(const std::string &d) {
        if (tcpConnPtr) {
            tcpConnPtr->Send(d);
        }
    }

    void send(const evpp::Slice &message) {
        if (tcpConnPtr) {
            tcpConnPtr->Send(message);
        }
    }

    void send(evpp::Buffer *buf) {
        if (tcpConnPtr) {
            tcpConnPtr->Send(buf);
        }
    }

    // 拒绝拷贝构造
    Server(const Server &rhs) = delete;

    // 拒绝拷贝赋值
    Server &operator=(const Server &rhs) = delete;
};

#endif //PITWINS_SERVER_H
