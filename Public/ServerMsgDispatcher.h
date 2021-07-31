//
// Created by Will Zhang on 2021/7/27.
//

#ifndef PITWINS_SERVERMSGDISPATCHER_H
#define PITWINS_SERVERMSGDISPATCHER_H

#include "MessageDispatcher.h"
#include "CameraMsgProcessor.h"
#include "ServoMsgProcessor.h"
#include "HeartbeatMsgProcessor.h"

using namespace PiRPC;

class ServerMsgDispatcher : public PiRPC::MessageDispatcher {
private:

public:
    ServerMsgDispatcher() = default;

    ~ServerMsgDispatcher() {
        processorMapping.empty();
    }

    static ServerMsgDispatcher &getInstance() {
        static ServerMsgDispatcher instance;
        return instance;
    }

    // 拒绝拷贝构造
    ServerMsgDispatcher(const ServerMsgDispatcher &rhs) = delete;

    // 拒绝拷贝赋值
    ServerMsgDispatcher &operator=(const ServerMsgDispatcher &rhs) = delete;

    void dispatch(const TCPConnPtr &connPtr, nlohmann::json msg) override {
        try {
            MessageProcessor *processor = getOrCreateProcessor(connPtr, msg["type"]);
            if (nullptr != processor) {
                processor->process(msg);
            } else {
                MessageProcessor::processUnknownMessage(msg.dump(), "No such msg processor");
            }
        } catch (std::exception &e) {
            MessageProcessor::processUnknownMessage(msg.dump(), e.what());
        }
    }

    MessageProcessor *getOrCreateProcessor(const TCPConnPtr &connPtr, int type) {
        MessageProcessor *processor = processorMapping[type];
        if (nullptr == processor) {
            switch (type) {
                case TYPE_HEARTBEAT:
                    processor = new HeartbeatMsgProcessor();
                    ((HeartbeatMsgProcessor*)processor)->updateConnectionMap(connPtr->id());
                    break;
                case TYPE_CAMERA_CTRL:
                    processor = new CameraMsgProcessor();
                    break;
                case TYPE_SERVO_CTRL:
                    processor = new ServoMsgProcessor();
                    break;
                case TYPE_MOTOR_CTRL:
                    spdlog::info("No impl yet:{}", type);
                    // processor = new ServoMsgProcessor();
                    break;
                default:
                    spdlog::warn("No such msg processor:{}", type);
                    break;
            }
            processorMapping[type] = processor;
        }
        return processor;
    }
};

#endif //PITWINS_SERVERMSGDISPATCHER_H
