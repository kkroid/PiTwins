//
// Created by Will Zhang on 2021/7/15.
//

#ifndef PITWINS_CAMERAMSGPROCESSOR_H
#define PITWINS_CAMERAMSGPROCESSOR_H

#include "MessageProcessor.h"

class CameraMsgProcessor : public MessageProcessor {
public:
    void process(nlohmann::json msg) override;
};

#endif //PITWINS_CAMERAMSGPROCESSOR_H
