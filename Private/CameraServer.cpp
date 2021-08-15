//
// Created by Will Zhang on 2021/7/15.
//
#include "CameraServer.h"
#include <future>
#include "MsgGen.h"
#include "CameraMsgProcessor.h"

using namespace PiRPC;

void CameraServer::open(int id) {
    spdlog::info("Video server call open");
    if (!Server::getVideoInstance().isRunning()) {
        threadPool->push([](int id) {
            spdlog::info("Video server not running, start it");
            Server::getVideoInstance().init(ADDR_VIDEO_SERVER, NAME_VIDEO_SERVER);
            Server::getVideoInstance().run();
        });
    }
    Server::getMsgInstance().send(MsgGen::camera(CAMERA_CMD_OPENED, 0));
    if (isOpened) {
        spdlog::info("Camera is opened");
    } else {
        isOpened = true;
        videoCapture = new VideoCapture();
        if (videoCapture->open(id, CAP_V4L2)) {
            spdlog::info("CameraServer opened with camera id:{}", id);
        } else {
            spdlog::error("CameraServer opened with camera id:{} failed", id);
            return;
        }
        int ret = videoCapture->set(CAP_PROP_FPS, 30);
        spdlog::info("set fps success:{}, read:{}", ret, videoCapture->get(CAP_PROP_FPS));
        videoCapture->set(CAP_PROP_FRAME_WIDTH, 640);
        videoCapture->set(CAP_PROP_FRAME_HEIGHT, 480);
        videoCapture->set(CAP_PROP_AUTO_WB, 1);
        videoCapture->set(CAP_PROP_AUTOFOCUS, 1);
        videoCapture->set(CAP_PROP_AUTO_EXPOSURE, 1);

        threadPool->push([this](int id) {
            if (videoCapture->isOpened()) {
                streaming = true;
                spdlog::info("CameraServer started streaming");
                while (streaming) {
                    Mat frame;
                    videoCapture->read(frame);
                    onFrame(frame);
                }
            }
        });
    }
}

void CameraServer::close() {
    streaming = false;
    if (videoCapture) {
        videoCapture->release();
    }
    isOpened = false;
}

void CameraServer::onFrame(Mat frame) {
    if (!CameraServer::isStreaming()) {
        spdlog::warn("Ignore invalid frame, not streaming");
        frame.release();
        return;
    }
    // spdlog::info("streaming");
    vector<uchar> buff;
    // imencode(".webp", matFrame, buff, params);
    imencode(".jpg", frame, buff, params);
    Server::getVideoInstance().send(buff.data(), buff.size());
    frame.release();
}
