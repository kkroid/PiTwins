//
// Created by Will Zhang on 2021/7/27.
//

#ifndef PITWINS_SERVERMSGDISPATCHER_H
#define PITWINS_SERVERMSGDISPATCHER_H

#include "MessageDispatcher.h"
#include "CameraMsgProcessor.h"
#include "ServoMsgProcessor.h"


using namespace PiRPC;

class ServerMsgDispatcher : public PiRPC::MessageDispatcher {
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

    void dispatch(nlohmann::json msg) override {
        try {
            MessageProcessor *processor = getOrCreateProcessor(msg["type"]);
            if (nullptr != processor) {
                processor->process(msg);
            } else {
                MessageProcessor::processUnknownMessage(msg.dump(), "No such msg processor");
            }
        } catch (std::exception &e) {
            MessageProcessor::processUnknownMessage(msg.dump(), e.what());
        }
    }

    MessageProcessor *getOrCreateProcessor(int type) {
        MessageProcessor *processor = processorMapping[type];
        if (nullptr == processor) {
            switch (type) {
                case TYPE_HEARTBEAT:

                    break;
                case TYPE_CAMERA_CTRL:
                    processor = new CameraMsgProcessor();
                    processorMapping[type] = processor;
                    break;
                case TYPE_SERVO_CTRL:
                    processor = new ServoMsgProcessor();
                    processorMapping[type] = processor;
                    break;
                case TYPE_MOTOR_CTRL:
                    spdlog::info("No impl yet:{}", type);
                    // processor = new ServoMsgProcessor();
                    break;
                default:
                    spdlog::warn("No such msg processor:{}", type);
                    break;
            }
        }
        return processor;
    }
};

#endif //PITWINS_SERVERMSGDISPATCHER_H
