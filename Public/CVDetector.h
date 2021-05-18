//
// Created by Will Zhang on 2020/10/27.
//

#ifndef CVDETECTOR_H
#define CVDETECTOR_H

#include <opencv2/core.hpp>
#include <opencv2/objdetect.hpp>
#include <string>
#include <vector>

using namespace std;
using namespace cv;

class CascadeDetectorAdapter : public DetectionBasedTracker::IDetector {
public:
    explicit CascadeDetectorAdapter(const Ptr<CascadeClassifier>& detector) :
            IDetector(),
            detector(detector) {
        cout << "CascadeDetectorAdapter::Detect::Detect" << endl;
        CV_Assert(detector);
        scaleFactor = 1.1f;
    }

    void detect(const Mat &Image,
                std::vector<Rect> &objects) override {
        detector->detectMultiScale(Image,
                                   objects,
                                   scaleFactor,
                                   2,
                                   CASCADE_SCALE_IMAGE,
                                   Size(30, 30));
    }

    ~CascadeDetectorAdapter() override {
        cout << "CascadeDetectorAdapter::Detect::~Detect" << endl;
    }

private:
    CascadeDetectorAdapter() = default;

    Ptr<CascadeClassifier> detector;
};

struct DetectorAgregator {
    Ptr<CascadeDetectorAdapter> mainDetector;
    Ptr<CascadeDetectorAdapter> trackingDetector;

    Ptr<DetectionBasedTracker> tracker;

    DetectorAgregator(Ptr<CascadeDetectorAdapter> &_mainDetector,
                      Ptr<CascadeDetectorAdapter> &_trackingDetector) :
            mainDetector(_mainDetector),
            trackingDetector(_trackingDetector) {
        CV_Assert(_mainDetector);
        CV_Assert(_trackingDetector);

        DetectionBasedTracker::Parameters DetectorParams;
        tracker = makePtr<DetectionBasedTracker>(mainDetector, trackingDetector, DetectorParams);
    }
};

class CVDetector {

private:
    DetectorAgregator *detectorAgregator{};
public:

    CVDetector() = default;

    ~CVDetector() {
        delete detectorAgregator;
    }

    void create(const string& path) {
        try {
            Ptr<CascadeDetectorAdapter> mainDetector = makePtr<CascadeDetectorAdapter>(
                    makePtr<CascadeClassifier>(path));
            Ptr<CascadeDetectorAdapter> trackingDetector = makePtr<CascadeDetectorAdapter>(
                    makePtr<CascadeClassifier>(path));
            detectorAgregator = new DetectorAgregator(mainDetector, trackingDetector);
//            if (faceSize > 0)
//            {
            mainDetector->setMinObjectSize(Size(20, 20));
            trackingDetector->setMinObjectSize(Size(20, 20));
//            }
            detectorAgregator->tracker->run();
        } catch (const Exception &e) {
            cout << "nativeCreateObject caught Exception: %s" << e.what() << endl;
        } catch (...) {
            cout << "nativeCreateObject caught unknown exception" << endl;
        }
    }

    void destroy() {
        try {
            detectorAgregator->tracker->stop();
        } catch (const Exception &e) {
            cout << "nativeestroyObject caught Exception: " << e.what();
        } catch (...) {
            cout << "nativeDestroyObject caught unknown exception";
        }
    }

    void detect(const Mat& gray, vector<Rect> *faces) {
        detectorAgregator->tracker->process(gray);
        detectorAgregator->tracker->getObjects(*faces);
    }
};

#endif //CVDETECTOR_H
