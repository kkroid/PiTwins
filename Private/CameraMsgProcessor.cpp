//
// Created by Will Zhang on 2021/7/15.
//
#include "CameraMsgProcessor.h"
#include "CameraServer.h"

void CameraMsgProcessor::process(nlohmann::json msg) {
    int cmd = msg["payload"]["cmd"];
    switch (cmd) {
        case TYPE_OPEN_CAMERA:
            CameraServer::getInstance().open(msg["payload"]["param"]);
            break;
        case TYPE_CLOSE_CAMERA:
            CameraServer::getInstance().close();
            break;
        default:
            spdlog::warn("no such cmd:{}", cmd);
            break;
    }
}

