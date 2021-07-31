//
// Created by Will Zhang on 2021/7/31.
//

#include "HeartbeatMsgProcessor.h"
#include "json.hpp"

void HeartbeatMsgProcessor::process(nlohmann::json msg) {
    
}

void HeartbeatMsgProcessor::updateConnectionMap(uint64_t id) {
    _connectionMap[id] = Snowflake::GetTimeStamp();
}
