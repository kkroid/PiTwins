//
// Created by Will Zhang on 2021/7/31.
//

#ifndef PITWINS_HEARTBEATMSGPROCESSOR_H
#define PITWINS_HEARTBEATMSGPROCESSOR_H

#include "commom/MessageProcessor.h"
#include "commom/Snowflake.h"

using namespace PiRPC;

class HeartbeatMsgProcessor : public PiRPC::MessageProcessor {
public:
    void process(nlohmann::json msg) override;
};

#endif //PITWINS_HEARTBEATMSGPROCESSOR_H
