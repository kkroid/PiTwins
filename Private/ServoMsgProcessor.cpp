//
// Created by Will Zhang on 2021/7/15.
//
#if Pi
#include "ServoMsgProcessor.h"

void ServoMsgProcessor::process(nlohmann::json msg) {
    int servo = msg["payload"]["servo"];
    int delta = msg["payload"]["delta"];
    servoAngle[servo - 1] = servoAngle[servo - 1] + delta;
    int angle = servoAngle[servo - 1];
    spdlog::info("servo[{}] new angle:{}", servo, angle);
    pwm.getServo(servo)->writeServo(angle);
}
#endif
