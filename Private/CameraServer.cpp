//
// Created by Will Zhang on 2021/7/15.
//
#include "CameraServer.h"
#include <future>
#include "commom/MsgGen.h"
#include "CameraMsgProcessor.h"

#define ADDR_VIDEO_SERVER "0.0.0.0:5555"
#define NAME_VIDEO_SERVER "VideoServer"

using namespace PiRPC;

void CameraServer::open(int id) {
    spdlog::info("Video server call open:{}", id);
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
        try {
            videoCapture = VideoCapture();
#if Pi
            if (videoCapture.open(id, CAP_V4L2)) {
#else
            if (videoCapture.open(id)) {
#endif
                spdlog::info("CameraServer opened with camera id:{}", id);
            } else {
                spdlog::error("CameraServer opened with camera id:{} failed", id);
                return;
            }
        } catch (std::exception &e) {
            spdlog::error("Failed to open camera:{}", e.what());
            return;
        }
        int ret = videoCapture.set(CAP_PROP_FPS, 30);
        spdlog::info("set fps success:{}, read:{}", ret, videoCapture.get(CAP_PROP_FPS));
        videoCapture.set(CAP_PROP_FRAME_WIDTH, 640);
        videoCapture.set(CAP_PROP_FRAME_HEIGHT, 480);
        videoCapture.set(CAP_PROP_AUTO_WB, 1);
        videoCapture.set(CAP_PROP_AUTOFOCUS, 1);
        videoCapture.set(CAP_PROP_AUTO_EXPOSURE, 1);

        threadPool->push([this](int id) {
            if (videoCapture.isOpened()) {
                streaming = true;
                spdlog::info("CameraServer started streaming");
                cvDetector.create("../Data/lbpcascade_frontalface.xml");
                while (streaming) {
                    Mat frame;
                    videoCapture.read(frame);
                    onFrame(frame);
                }
            }
        });
    }
}

void CameraServer::close() {
    streaming = false;
    videoCapture.release();
    isOpened = false;
}

void CameraServer::detect(Mat &matFrame, vector<Rect> &rectFaces) {
    Mat grayImg, resizedMat;
    cvtColor(matFrame, grayImg, COLOR_BGR2GRAY);
    // 对缩小了2倍的图片进行检测
    int scale = 2;
    resize(grayImg, resizedMat, Size(grayImg.cols / scale, grayImg.rows / scale));
    cvDetector.detect(resizedMat, &rectFaces);
    // 宽高各缩小了2倍，检测结果需要相应的放大
    for (auto rect : rectFaces) {
        rect.x = rect.x * scale;
        rect.y = rect.y * scale;
        rect.width = rect.width * scale;
        rect.height = rect.height * scale;
        // draw face rect
        rectangle(matFrame, rect, Scalar(255, 0, 0), 1);
    }

    grayImg.release();
    resizedMat.release();
}

void CameraServer::onFrame(Mat frame) {
    if (!CameraServer::isStreaming()) {
        spdlog::warn("Ignore invalid frame, not streaming");
        frame.release();
        return;
    }
    // spdlog::info("streaming");
    vector<Rect> rectFaces;
    detect(frame, rectFaces);
    vector<uchar> buff;
    // imencode(".webp", matFrame, buff, params);
    imencode(".jpg", frame, buff, params);
    Server::getVideoInstance().send(buff.data(), buff.size());
    frame.release();
}
