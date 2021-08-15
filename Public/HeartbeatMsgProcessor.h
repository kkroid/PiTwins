//
// Created by Will Zhang on 2021/7/31.
//

#ifndef PITWINS_HEARTBEATMSGPROCESSOR_H
#define PITWINS_HEARTBEATMSGPROCESSOR_H

#include "commom/MessageProcessor.h"
#include "commom/Snowflake.h"

using namespace PiRPC;

class HeartbeatMsgProcessor : public PiRPC::MessageProcessor {
protected:
    std::map<uint64_t/*the id of the connection*/, UInt64> _connectionMap;
public:
    void process(nlohmann::json msg) override;

    void updateConnectionMap(uint64_t id);
};

#endif //PITWINS_HEARTBEATMSGPROCESSOR_H
