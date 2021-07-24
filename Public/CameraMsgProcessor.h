//
// Created by Will Zhang on 2021/7/15.
//

#ifndef PITWINS_CAMERAMSGPROCESSOR_H
#define PITWINS_CAMERAMSGPROCESSOR_H

#include "MessageProcessor.h"

#define CAMERA_CMD_OPEN 0
#define CAMERA_CMD_CLOSE 1
#define CAMERA_CMD_OPENED 2

class CameraMsgProcessor : public MessageProcessor {
public:
    void process(nlohmann::json msg) override;
};

#endif //PITWINS_CAMERAMSGPROCESSOR_H
