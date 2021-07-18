//
// Created by Will Zhang on 2021/7/15.
//

#ifndef PITWINS_CAMERASERVER_H
#define PITWINS_CAMERASERVER_H

#include <opencv2/opencv.hpp>
#include "spdlog/spdlog.h"
#include "ctpl_stl.h"
#include "Frame.h"
#include "Server.h"

using namespace std;
using namespace cv;


class CameraServer {
private:
    VideoCapture *videoCapture;
    bool streaming = false;
    ctpl::thread_pool *threadPool;
    vector<int> params;
    bool isOpened = false;
public:
    CameraServer() {
        videoCapture = nullptr;
        threadPool = new ctpl::thread_pool(2);
        // params.push_back(IMWRITE_WEBP_QUALITY);
        // params.push_back(75);
        params.push_back(IMWRITE_JPEG_QUALITY);
        params.push_back(60);
        // params.push_back(IMWRITE_JPEG_OPTIMIZE);
        // params.push_back(1);
    };

    ~CameraServer() {
        spdlog::info("~CameraServer");
        close();
        delete videoCapture;
        videoCapture = nullptr;
        delete threadPool;
        threadPool = nullptr;
    }

    static CameraServer &getInstance() {
        static CameraServer instance;
        return instance;
    }

    // 拒绝拷贝构造
    CameraServer(const CameraServer &rhs) = delete;

    // 拒绝拷贝赋值
    CameraServer &operator=(const CameraServer &rhs) = delete;

    void open(int id);

    void close();

    void onFrame(Mat frame);

    bool isStreaming() {
        return streaming;
    }

};

#endif //PITWINS_CAMERASERVER_H
