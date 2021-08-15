//
// Created by Will Zhang on 2021/7/15.
//
#include "CameraMsgProcessor.h"
#include "CameraServer.h"

void CameraMsgProcessor::process(nlohmann::json msg) {
    int cmd = msg["payload"]["cmd"];
    switch (cmd) {
        case CAMERA_CMD_OPEN:
            CameraServer::getInstance().open(msg["payload"]["param"]);
            break;
        case CAMERA_CMD_CLOSE:
            CameraServer::getInstance().close();
            break;
        default:
            spdlog::warn("CameraMsgProcessor:no such cmd:{}", cmd);
            break;
    }
}

