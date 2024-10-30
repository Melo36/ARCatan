#pragma once
// ocv_MarkerTracker.h

// solution presented in the AR class

#ifndef ocv_MarkerTracker_H
#define ocv_MarkerTracker_H

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

class CvMemStorage;

const std::string kWinName1 = "Exercise 8 - Original Image";
const std::string kWinName2 = "Exercise 8 - Converted Image";
const std::string kWinName3 = "Exercise 8 - Stripe Image";
const std::string kWinName4 = "Exercise 8 - Marker";

class MarkerTracker {
public:
    MarkerTracker(double kMarkerSize_)
        : thresh(100),
        bw_thresh(100),
        kMarkerSize(kMarkerSize_)
    {
        init();
    }
    MarkerTracker(double kMarkerSize_, int thresh_, int bw_thresh_)
        : thresh(thresh_),
        bw_thresh(bw_thresh_),
        kMarkerSize(kMarkerSize_)
    {
        init();
    }
    ~MarkerTracker() {
        cleanup();
    }
    int findAllMarkers_pnp(cv::Mat& img_bgr, std::vector<int>& MarkerID, std::vector<cv::Mat>& MarkerPmat);

    /*--- NEW FOR PnP ---*/
    cv::Mat KMatrix() { return _Kmat; }

protected:
    void init();
    void cleanup();

    //camera settings
    const double kMarkerSize; // Marker size [m]

    cv::Mat img_gray;
    cv::Mat img_mono;

    int thresh; // Threshold (gray to mono)
    int bw_thresh; // threshold for (gray maker to ID image)

    /*--- NEW FOR PnP begin ---*/
    cv::Mat _Kmat = (cv::Mat_<double>(3, 3) <<
        1., 0., 0.,
        0., 1., 0.,
        0., 0., 1.);
    /*--- NEW FOR PnP end ---*/

    CvMemStorage* memStorage;
};

#endif // ocv_MarkerTracker_H
