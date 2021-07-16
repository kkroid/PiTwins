//
// Created by Will Zhang on 2021/7/15.
//
#include "CameraServer.h"
#include "Server.h"

void CameraServer::open(int id) {
    if (isOpened) {
        spdlog::info("Camera is opened");
    } else {
        isOpened = true;
        videoCapture = new VideoCapture(id);
        spdlog::info("CameraServer opened with camera id:{}", id);
        // videoCapture = new VideoCapture("http://127.0.0.1:8081/?action=stream");
        int ret = videoCapture->set(CAP_PROP_FPS, 30);
        spdlog::info("set fps success:{}, read:{}", ret, videoCapture->get(CAP_PROP_FPS));
        videoCapture->set(CAP_PROP_FRAME_WIDTH, 640);
        videoCapture->set(CAP_PROP_FRAME_HEIGHT, 480);
        videoCapture->set(CAP_PROP_AUTO_WB, 1);
        videoCapture->set(CAP_PROP_AUTOFOCUS, 1);
        videoCapture->set(CAP_PROP_AUTO_EXPOSURE, 1);
    }
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
    Frame *f = compressFrameData(frame);
    Server::getInstance().send(f->data, f->size);
    // spdlog::info("Got a frame:{}", f->size);
    delete f;
    frame.release();
}

Frame *CameraServer::compressFrameData(Mat &matFrame) {
    vector<uchar> buff;
    // spdlog::info("before compressFrameData:{}", matFrame.total() * matFrame.elemSize());
    // imencode(".webp", matFrame, buff, params);
    imencode(".jpg", matFrame, buff, params);
    auto *data = new uchar[buff.size()];
    copy(buff.begin(), buff.end(), data);
    // spdlog::info("after compressFrameData:{}", sizeof(uchar) * buff.size());
    return new Frame(data, sizeof(uchar) * buff.size());
}