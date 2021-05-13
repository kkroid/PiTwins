//
// Created by Will Zhang on 2021/1/10.
//

#ifndef PITWINS_VIDEOPUBLISHER_H
#define PITWINS_VIDEOPUBLISHER_H

#include "Publisher.h"
#include "Frame.h"
#include <opencv2/opencv.hpp>
#include "CVDetector.h"

using namespace std;
using namespace cv;

class VideoPublisher : public Publisher {
private:
    VideoCapture *videoCapture{};
public:
    CVDetector cvDetector;
    vector<int> params;

public:
    explicit VideoPublisher(socket_t *publisher) : Publisher(publisher) {
        spdlog::info("VideoPublisher created");
        cvDetector.create("../data/lbpcascade_frontalface.xml");
        // params.push_back(IMWRITE_WEBP_QUALITY);
        // params.push_back(100);
        params.push_back(IMWRITE_JPEG_QUALITY);
        params.push_back(60);
        // params.push_back(IMWRITE_JPEG_OPTIMIZE);
        // params.push_back(1);
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
        loop();
    }

    void loop() {
        if (videoCapture->isOpened()) {
            for (;;) {
                Mat matFrame;
                videoCapture->read(matFrame);
                if (matFrame.empty()) {
                    spdlog::info("VideoPublisher got an empty mat frame, ignore");
                    matFrame.release();
                    break;
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
};


#endif //PITWINS_VIDEOPUBLISHER_H
