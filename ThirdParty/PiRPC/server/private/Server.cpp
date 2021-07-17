//
// Created by Will Zhang on 2021/7/10.
//

#include "Server.h"
#include "MessageReceiver.h"


void Server::run() {
    if (nullptr != server) {
        spdlog::info("{} is running", name);
        server->Init();
        server->Start();
        loop->Run();
    }
}

void Server::release() {
    spdlog::info("call {} stop", name);
    server->Stop([this]() {
        spdlog::info("{} stopped", name);
    });
    loop->Stop();
    delete loop;
    loop = nullptr;
    delete server;
    server = nullptr;
}

void Server::setConnectionCallback(const evpp::ConnectionCallback &ccb) {
    if (nullptr != server) {
        server->SetConnectionCallback([this, ccb](const evpp::TCPConnPtr &connPtr) {
            if (connPtr->IsConnected()) {
                spdlog::info("Client {} Connected", connPtr->remote_addr());
                tcpConnPtr = connPtr;
            } else if (connPtr->IsDisconnected()) {
                spdlog::info("Client {} Disconnected", connPtr->remote_addr());
                tcpConnPtr = nullptr;
            }
            if (ccb) {
                ccb(connPtr);
            }
        });
    }
}

void Server::setMessageCallback(const evpp::MessageCallback &mcb) {
    if (nullptr != server) {
        server->SetMessageCallback([this, mcb](const evpp::TCPConnPtr &connPtr, evpp::Buffer *buffer) {
            MessageReceiver::getInstance().onNewMsgReceived(connPtr, buffer);
            if (mcb) {
                mcb(connPtr, buffer);
            }
        });
    }
}
