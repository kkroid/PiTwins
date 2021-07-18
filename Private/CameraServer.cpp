//
// Created by Will Zhang on 2021/7/15.
//
#include "CameraServer.h"
#include <future>

void CameraServer::open(int id) {
    auto videoServer = async(launch::async, []() {
        if (Server::getVideoInstance().isRunning()) {
            return;
        }
        spdlog::info("Video server not running, start it");
        Server::getVideoInstance().init();
        Server::getVideoInstance().run();
    });
    if (isOpened) {
        spdlog::info("Camera is opened");
    } else {
        isOpened = true;
        videoCapture = new VideoCapture(id);
        spdlog::info("CameraServer opened with camera id:{}", id);
        // videoCapture = new VideoCapture("http://127.0.0.1:8081/?action=stream");
        int ret = videoCapture->set(CAP_PROP_FPS, 15);
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

    vector<uchar> buff;
    // imencode(".webp", matFrame, buff, params);
    imencode(".jpg", frame, buff, params);
    Server::getVideoInstance().send(buff.data(), buff.size());
    frame.release();
}
