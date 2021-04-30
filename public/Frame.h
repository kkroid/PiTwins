#ifndef FRAME_H
#define FRAME_H

#include <cstdint>
#include <opencv2/opencv.hpp>

using namespace cv;


class Frame {
public:
    uchar *data;
    u_long size = 0;

    Frame(uchar *data, u_long len) : data(data), size(len) {
    }

    ~Frame() {
        delete data;
    }
};

#endif //FRAME_H
