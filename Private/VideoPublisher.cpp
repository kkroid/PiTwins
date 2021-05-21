//
// Created by Will Zhang on 2021/1/10.
//

#include "VideoPublisher.h"

using namespace std;


void VideoPublisher::detect(Mat &matFrame, vector<Rect> &rectFaces) {
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

Frame *VideoPublisher::compressFrameData(Mat &matFrame) {
    vector<uchar> buff;
    // imencode(".webp", matFrame, buff, params);
    imencode(".jpg", matFrame, buff, params);
    auto *data = new uchar[buff.size()];
    copy(buff.begin(), buff.end(), data);
    return new Frame(data, sizeof(uchar) * buff.size());
}


