//
// Created by Will Zhang on 2021/1/10.
//

#ifndef PITWINS_VIDEOPUBLISHER_H
#define PITWINS_VIDEOPUBLISHER_H

#include "Publisher.h"
#include "Frame.h"
#include <opencv2/opencv.hpp>
#include "CVDetector.h"

#ifdef Pi

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
#endif

using namespace std;
using namespace cv;

class VideoPublisher : public Publisher {
private:
    VideoCapture *videoCapture{};
    bool streaming = true;
    socket_t *cmdReceiver;
    MotorShield pwm;
public:
    CVDetector cvDetector;
    vector<int> params;

public:
    explicit VideoPublisher(socket_t *publisher, socket_t *cmdReceiver) : Publisher(publisher) {
        cvDetector.create("../Data/lbpcascade_frontalface.xml");
        // params.push_back(IMWRITE_WEBP_QUALITY);
        // params.push_back(100);
        params.push_back(IMWRITE_JPEG_QUALITY);
        params.push_back(60);
        // params.push_back(IMWRITE_JPEG_OPTIMIZE);
        // params.push_back(1);
        this->cmdReceiver = cmdReceiver;
        pwm = MotorShield(I2C_ADDR);
        pwm.begin(HERTZ);
        spdlog::info("VideoPublisher created");
    }

    ~VideoPublisher() override {
        spdlog::info("VideoPublisher released");
        if (videoCapture) {
            videoCapture->release();
        }
        delete videoCapture;
        cvDetector.destroy();
    }

    void start(char *id) override {
        spdlog::info("VideoPublisher started:{}", id);
        videoCapture = new VideoCapture(id == nullptr ? -1 : atoi(id));
        // videoCapture = new VideoCapture("http://127.0.0.1:8081/?action=stream");
        int ret = videoCapture->set(CAP_PROP_FPS, 30);
        spdlog::info("set fps success:{}, read:{}", ret, videoCapture->get(CAP_PROP_FPS));
        videoCapture->set(CAP_PROP_FRAME_WIDTH, 640);
        videoCapture->set(CAP_PROP_FRAME_HEIGHT, 480);
        videoCapture->set(CAP_PROP_AUTO_WB, 1);
        videoCapture->set(CAP_PROP_AUTOFOCUS, 1);
        videoCapture->set(CAP_PROP_AUTO_EXPOSURE, 1);
#ifdef Pi
        threadPool->push([this](int id) {
            this->handleCmd();
        });
#endif
        loop();
    }

    void loop() {
        if (videoCapture->isOpened()) {
            while (streaming) {
                Mat matFrame;
                videoCapture->read(matFrame);
                if (matFrame.empty()) {
                    spdlog::info("VideoPublisher got an empty mat frame, ignore");
                    matFrame.release();
                    continue;
                }
                // vector<Rect> rectFaces;
                // detect(matFrame, rectFaces);
                Frame *frame = compressFrameData(matFrame);
                matFrame.release();
                publish(const_buffer(frame->data, frame->size), send_flags::none);
                delete frame;
            }
        }
    }

    virtual Frame *compressFrameData(Mat &matFrame);

    virtual void detect(Mat &matFrame, vector<Rect> &rectFaces);

#ifdef Pi

    void handleCmd() {
        uint8_t servo1Angle = 90;
        uint8_t server2Angle = 90;
        while (streaming) {
            zmq::message_t msg;
            const auto ret = cmdReceiver->recv(msg, recv_flags::none);
            if (!ret) {
                spdlog::error("CMDReceiver received error:{}", ret.value());
                break;
            }
            spdlog::info("CMDReceiver received:{}", msg.to_string());
            nlohmann::json obj = nlohmann::json::parse(msg.to_string());
            int type = obj["type"];
            int key = obj["payload"]["key"];
            int value = obj["payload"]["value"];
            if (type == TYPE_SERVO) {
                switch (key) {
                    case 1:
                        server2Angle--;
                        pwm.getServo(SERVO_VERTICAL)->writeServo(server2Angle);
                        break;
                    case 2:
                        server2Angle++;
                        pwm.getServo(SERVO_VERTICAL)->writeServo(server2Angle);
                        break;
                    case 3:
                        servo1Angle++;
                        pwm.getServo(SERVO_HORIZONTAL)->writeServo(servo1Angle);
                        break;
                    case 4:
                        servo1Angle--;
                        pwm.getServo(SERVO_HORIZONTAL)->writeServo(servo1Angle);
                        break;
                    default:
                        break;
                }
            } else if (type == TYPE_DCMOTOR) {
                value = value >= 0 && value <= 255 ? value : 255;
                /*********
                 * 3 1 4 *
                 * ↖ ↑ ↗ *
                 * ↙ ↓ ↘ *
                 * 5 2 6 *
                 *********/
                switch (key) {
                    case 1:
                        forward(value);
                        break;
                    case 2:
                        backward(value);
                        break;
                    case 3:
                        leftFront(value);
                        break;
                    case 4:
                        rightFront(value);
                        break;
                    case 5:
                        leftBack(value);
                        break;
                    case 6:
                        rightBack(value);
                        break;
                    case 7:
                        motorBrake();
                        break;
                    case 8:
                        motorRelease();
                        break;
                    default:
                        break;
                }
            } else {
                spdlog::warn("Invalid type:{}, key:{}, value:{}", type, key, value);
            }
            spdlog::info("write servoAngle for key {}:{}, {}", key, servo1Angle, server2Angle);
        }
        cmdReceiver->close();
    }

    void forward(uint8_t speed = 255) {
        pwm.getMotor(LEFT_MOTOR)->setSpeed(speed).run(FORWARD);
        pwm.getMotor(RIGHT_MOTOR)->setSpeed(speed).run(FORWARD);
    }

    void backward(uint8_t speed = 255) {
        pwm.getMotor(LEFT_MOTOR)->setSpeed(speed).run(BACKWARD);
        pwm.getMotor(RIGHT_MOTOR)->setSpeed(speed).run(BACKWARD);
    }

    void leftFront(uint8_t speed = 255) {
        pwm.getMotor(LEFT_MOTOR)->setSpeed(speed / 2).run(BRAKE);
        pwm.getMotor(RIGHT_MOTOR)->setSpeed(speed).run(FORWARD);
    }

    void rightFront(uint8_t speed = 255) {
        pwm.getMotor(LEFT_MOTOR)->setSpeed(speed).run(FORWARD);
        pwm.getMotor(RIGHT_MOTOR)->setSpeed(speed / 2).run(BRAKE);
    }

    void leftBack(uint8_t speed = 255) {
        pwm.getMotor(LEFT_MOTOR)->setSpeed(speed / 2).run(BACKWARD);
        pwm.getMotor(RIGHT_MOTOR)->setSpeed(speed).run(BACKWARD);
    }

    void rightBack(uint8_t speed = 255) {
        pwm.getMotor(LEFT_MOTOR)->setSpeed(speed).run(BACKWARD);
        pwm.getMotor(RIGHT_MOTOR)->setSpeed(speed / 2).run(BACKWARD);
    }

    void motorBrake() {
        pwm.getMotor(LEFT_MOTOR)->run(BRAKE);
        pwm.getMotor(RIGHT_MOTOR)->run(BRAKE);
    }

    void motorRelease() {
        pwm.getMotor(LEFT_MOTOR)->run(RELEASE);
        pwm.getMotor(RIGHT_MOTOR)->run(RELEASE);
    }

#endif
};


#endif //PITWINS_VIDEOPUBLISHER_H
