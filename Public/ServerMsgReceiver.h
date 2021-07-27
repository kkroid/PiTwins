//
// Created by Will Zhang on 2021/7/27.
//

#ifndef PITWINS_SERVERMSGRECEIVER_H
#define PITWINS_SERVERMSGRECEIVER_H

#include "MessageReceiver.h"
#include "CameraMsgProcessor.h"
#include "ServoMsgProcessor.h"


using namespace PiRPC;

class ServerMsgReceiver : public PiRPC::MessageReceiver {
public:
    ServerMsgReceiver() = default;

    ~ServerMsgReceiver() {
        processorMapping.empty();
    }

    static ServerMsgReceiver &getInstance() {
        static ServerMsgReceiver instance;
        return instance;
    }

    // 拒绝拷贝构造
    ServerMsgReceiver(const ServerMsgReceiver &rhs) = delete;

    // 拒绝拷贝赋值
    ServerMsgReceiver &operator=(const ServerMsgReceiver &rhs) = delete;

    void onNewMsgReceived(const std::string& msg) override {
        try {
            nlohmann::json obj = nlohmann::json::parse(msg);
            MessageProcessor *processor = getOrCreateProcessor(obj["type"]);
            processor->process(obj);
        } catch (nlohmann::detail::exception &e) {
            MessageProcessor::processUnknownMessage(msg, e.what());
        }
    }

    MessageProcessor *getOrCreateProcessor(int type) {
        MessageProcessor *processor = processorMapping[type];
        if (nullptr == processor) {
            switch (type) {
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

#endif //PITWINS_SERVERMSGRECEIVER_H
