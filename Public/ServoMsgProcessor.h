//
// Created by Will Zhang on 2021/7/15.
//

#ifndef PITWINS_SERVOMSGPROCESSOR_H
#define PITWINS_SERVOMSGPROCESSOR_H

#include "MessageProcessor.h"
#include "MotorShield.h"
#include "json.hpp"

#define I2C_ADDR 0x60
#define HERTZ 50
#define SERVO_HORIZONTAL 1
#define SERVO_VERTICAL 2
#define ZMQ_PULL_ADDR "tcp://*:5556"

#define TYPE_SERVO 1
#define TYPE_DCMOTOR 2
#define TYPE_MOTOR_ADVANCE 3

#define LEFT_MOTOR 1
#define RIGHT_MOTOR 2

class ServoMsgProcessor : public PiRPC::MessageProcessor {
public:
    ServoMsgProcessor() {
        pwm = MotorShield(I2C_ADDR);
        pwm.begin(HERTZ);
    }

private:
    MotorShield pwm;
    uint8_t servoAngle[2] = {90, 90};

    void process(nlohmann::json msg) override;
};

#endif //PITWINS_SERVOMSGPROCESSOR_H
