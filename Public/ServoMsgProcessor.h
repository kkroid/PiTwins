//
// Created by Will Zhang on 2021/7/15.
//

#ifndef PITWINS_SERVOMSGPROCESSOR_H
#define PITWINS_SERVOMSGPROCESSOR_H

#include "MessageProcessor.h"

class ServoMsgProcessor : public MessageProcessor {
    void process(nlohmann::json msg) override;
};

#endif //PITWINS_SERVOMSGPROCESSOR_H
