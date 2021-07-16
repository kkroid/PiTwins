//
// Created by Will Zhang on 2021/7/10.
//

#include "Server.h"

#include <utility>


void Server::run() {
    if (nullptr != server) {
        server->Init();
        server->Start();
        loop->Run();
    }
}
