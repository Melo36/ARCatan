// ocv_MarkerTracker.cpp

// solution presented in the AR class

#include <iostream>
#include <iomanip>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>  // NEW FOR PnP

#define _USE_MATH_DEFINES
#include <math.h>

#include "MarkerTracker_PnP.h"
//#include "ocv_PoseEstimation.h"

// List of points
typedef vector<Point> contour_t;
// List of contours
typedef vector<contour_t> contour_vector_t;

void trackbarHandler(int pos, void* slider_value) {
    *((int*)slider_value) = pos;
}

void bw_trackbarHandler(int pos, void* slider_value) {
    *((int*)slider_value) = pos;
}


int subpixSampleSafe(const cv::Mat& pSrc, const cv::Point2f& p)
{
    int x = int(floorf(p.x));
    int y = int(floorf(p.y));

    if (x < 0 || x >= pSrc.cols - 1 ||
        y < 0 || y >= pSrc.rows - 1)
        return 127;

    int dx = int(256 * (p.x - floorf(p.x)));
    int dy = int(256 * (p.y - floorf(p.y)));

    unsigned char* i = (unsigned char*)((pSrc.data + y * pSrc.step) + x);
    int a = i[0] + ((dx * (i[1] - i[0])) >> 8);
    i += pSrc.step;
    int b = i[0] + ((dx * (i[1] - i[0])) >> 8);
    return a + ((dy * (b - a)) >> 8);
}


void MarkerTracker::init()
{
    std::cout << "Startup\n";
    //cv::namedWindow(kWinName1, CV_WINDOW_AUTOSIZE);
    //cv::namedWindow(kWinName2, CV_WINDOW_AUTOSIZE);
    //cv::namedWindow(kWinName3, CV_WINDOW_AUTOSIZE);
    //cv::namedWindow(kWinName4, 0);
    //cvResizeWindow("Marker", 120, 120);

    //int max = 255;
    //int slider_value = 100;
    //cv::createTrackbar("Threshold", kWinName2, &slider_value, 255, trackbarHandler, &slider_value);

    //int bw_sileder_value = bw_thresh;
    //cv::createTrackbar("BW Threshold", kWinName2, &slider_value, 255, bw_trackbarHandler, &bw_sileder_value);

    //memStorage = cvCreateMemStorage();
}

void MarkerTracker::cleanup()
{
    //cvReleaseMemStorage(&memStorage);

    //cv::destroyWindow(kWinName1);
    //cv::destroyWindow(kWinName2);
    //cv::destroyWindow(kWinName3);
    //cv::destroyWindow(kWinName4);
    std::cout << "Finished\n";
}

int MarkerTracker::findAllMarkers_pnp(cv::Mat& img_bgr, std::vector<int>& MarkerID, std::vector<cv::Mat>& MarkerPmat) //, std::vector<float*> &MarkerPose)
{
    //    std::vector<float*> MarkerPose;
    //    std::vector<cv::Mat> MarkerPmat;
    float resultMatrix[16];

    bool isFirstStripe = true;

    bool isFirstMarker = true;

    //int markers = 0;
    //unsigned int markerID = std::stoul(markerIDstr, nullptr, 16);

    {
        cv::cvtColor(img_bgr, img_gray, COLOR_BGR2GRAY);
        cv::threshold(img_gray, img_mono, thresh, 255, THRESH_BINARY);
        
        contour_vector_t contours;
        
        // RETR_LIST is a list of all found contour, SIMPLE is to just save the begin and ending of each edge which belongs to the contour
        findContours(img_mono, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
        
        //int resultcode = 0;
        for (size_t k = 0; k < contours.size(); k++) {
            {
                contour_t approx_contour;
                
                Scalar QUADRILATERAL_COLOR(0, 0, 255);
                Scalar colour;
                
                // Simplifying of the contour with the Ramer-Douglas-Peuker Algorithm
                // true -> Only closed contours
                // Approximation of old curve, the difference (epsilon) should not be bigger than: perimeter(->arcLength)*0.02
                approxPolyDP(contours[k], approx_contour, arcLength(contours[k], true) * 0.02, true);
                
                // Convert to a usable rectangle
                Rect r = boundingRect(approx_contour);
                
                // 4 Corners -> We color them
                if (approx_contour.size() == 4) {
                    colour = QUADRILATERAL_COLOR;
                }
                else {
                    continue;
                }
                
                if (r.height<20 || r.width<20 || r.width > img_mono.cols - 10 || r.height > img_mono.rows - 10) {
                    continue;
                }
                
                //const cv::Point* rect = (const cv::Point*)result_.data;
                //int npts = result_.rows;
                // draw the polygon
                polylines(img_bgr, approx_contour, true, colour, 2);
                
                
                float lineParams[16];
                cv::Mat lineParamsMat(cv::Size(4, 4), CV_32F, lineParams); // lineParams is shared
                
                for (int i = 0; i < 4; ++i)
                {
                    circle(img_bgr, approx_contour[i], 3, CV_RGB(0, 255, 0), -1);
                    
                    double dx = ((double)approx_contour[(i + 1) % 4].x - (double)approx_contour[i].x) / 7.0;
                    double dy = ((double)approx_contour[(i + 1) % 4].y - (double)approx_contour[i].y) / 7.0;
                    
                    int stripeLength = (int)(0.8 * sqrt(dx * dx + dy * dy));
                    if (stripeLength < 5)
                        stripeLength = 5;
                    
                    //make stripeLength odd (because of the shift in nStop)
                    stripeLength |= 1;
                    
                    //e.g. stripeLength = 5 --> from -2 to 2
                    int nStop = stripeLength >> 1;
                    int nStart = -nStop;
                    
                    cv::Size stripeSize;
                    stripeSize.width = 3;
                    stripeSize.height = stripeLength;
                    
                    cv::Point2f stripeVecX;
                    cv::Point2f stripeVecY;
                    
                    //normalize vectors
                    double diffLength = sqrt(dx * dx + dy * dy);
                    stripeVecX.x = dx / diffLength;
                    stripeVecX.y = dy / diffLength;
                    
                    stripeVecY.x = stripeVecX.y;
                    stripeVecY.y = -stripeVecX.x;
                    
                    cv::Mat iplStripe(stripeSize, CV_8UC1);
                    
                    // Array for edge point centers
                    cv::Point2f points[6];
                    
                    for (int j = 1; j < 7; ++j)
                    {
                        double px = (double)approx_contour[i].x + (double)j * dx;
                        double py = (double)approx_contour[i].y + (double)j * dy;
                        
                        cv::Point p;
                        p.x = (int)px;
                        p.y = (int)py;
                        cv::circle(img_bgr, p, 2, CV_RGB(0, 0, 255), -1);
                        
                        for (int m = -1; m <= 1; ++m)
                        {
                            for (int n = nStart; n <= nStop; ++n)
                            {
                                cv::Point2f subPixel;
                                
                                subPixel.x = (double)p.x + ((double)m * stripeVecX.x) + ((double)n * stripeVecY.x);
                                subPixel.y = (double)p.y + ((double)m * stripeVecX.y) + ((double)n * stripeVecY.y);
                                
                                cv::Point p2;
                                p2.x = (int)subPixel.x;
                                p2.y = (int)subPixel.y;
                                
                                if (isFirstStripe)
                                    cv::circle(img_bgr, p2, 1, CV_RGB(255, 0, 255), -1);
                                else
                                    cv::circle(img_bgr, p2, 1, CV_RGB(0, 255, 255), -1);
                                
                                int pixel = subpixSampleSafe(img_gray, subPixel);
                                
                                int w = m + 1; //add 1 to shift to 0..2
                                int h = n + (stripeLength >> 1); //add stripelenght>>1 to shift to 0..stripeLength
                                
                                
                                iplStripe.at<uchar>(h, w) = (uchar)pixel;
                            }
                        }
                        
                        //use sobel operator on stripe
                        // ( -1 , -2, -1 )
                        // (  0 ,  0,  0 )
                        // (  1 ,  2,  1 )
                        std::vector<double> sobelValues(stripeLength - 2);
                        for (int n = 1; n < (stripeLength - 1); n++)
                        {
                            unsigned char* stripePtr = &(iplStripe.at<uchar>(n - 1, 0));
                            double r1 = -stripePtr[0] - 2 * stripePtr[1] - stripePtr[2];
                            stripePtr += 2 * iplStripe.step;
                            double r3 = stripePtr[0] + 2 * stripePtr[1] + stripePtr[2];
                            sobelValues[n - 1] = r1 + r3;
                        }
                        
                        double maxVal = -1;
                        int maxIndex = 0;
                        for (int n = 0; n < stripeLength - 2; ++n)
                        {
                            if (sobelValues[n] > maxVal)
                            {
                                maxVal = sobelValues[n];
                                maxIndex = n;
                            }
                        }
                        
                        double y0, y1, y2; // y0 .. y1 .. y2
                        y0 = (maxIndex <= 0) ? 0 : sobelValues[maxIndex - 1];
                        y1 = sobelValues[maxIndex];
                        y2 = (maxIndex >= stripeLength - 3) ? 0 : sobelValues[maxIndex + 1];
                        
                        //formula for calculating the x-coordinate of the vertex of a parabola, given 3 points with equal distances
                        //(xv means the x value of the vertex, d the distance between the points):
                        //xv = x1 + (d / 2) * (y2 - y0)/(2*y1 - y0 - y2)
                        
                        double pos = (y2 - y0) / (4 * y1 - 2 * y0 - 2 * y2); //d = 1 because of the normalization and x1 will be added later
                        
                        
                        
                        if (pos != pos) {
                            // value is not a number
                            continue;
                        }
                        
                        cv::Point2f edgeCenter; //exact point with subpixel accuracy
                        int maxIndexShift = maxIndex - (stripeLength >> 1);
                        
                        //shift the original edgepoint accordingly
                        edgeCenter.x = (double)p.x + (((double)maxIndexShift + pos) * stripeVecY.x);
                        edgeCenter.y = (double)p.y + (((double)maxIndexShift + pos) * stripeVecY.y);
                        
                        cv::Point p_tmp;
                        p_tmp.x = (int)edgeCenter.x;
                        p_tmp.y = (int)edgeCenter.y;
                        cv::circle(img_bgr, p_tmp, 1, CV_RGB(0, 0, 255), -1);
                        
                        points[j - 1].x = edgeCenter.x;
                        points[j - 1].y = edgeCenter.y;
                        
                        if (isFirstStripe)
                        {
                            cv::Mat iplTmp;
                            cv::resize(iplStripe, iplTmp, cv::Size(100, 300));
                            //cv::imshow(kWinName3, iplTmp);//iplStripe );
                            isFirstStripe = false;
                        }
                        
                    } // end of loop over edge points of one edge
                    
                    // we now have the array of exact edge centers stored in "points"
                    cv::Mat mat(cv::Size(1, 6), CV_32FC2, points);
                    cv::fitLine(mat, lineParamsMat.col(i), DIST_L2, 0, 0.01, 0.01);
                    // cvFitLine stores the calculated line in lineParams in the following way:
                    // vec.x, vec.y, point.x, point.y
                    
                    cv::Point p;
                    p.x = (int)lineParams[8 + i] - (int)(50.0 * lineParams[i]);
                    p.y = (int)lineParams[12 + i] - (int)(50.0 * lineParams[4 + i]);
                    
                    cv::Point p2;
                    p2.x = (int)lineParams[8 + i] + (int)(50.0 * lineParams[i]);
                    p2.y = (int)lineParams[12 + i] + (int)(50.0 * lineParams[4 + i]);
                    
                    cv::line(img_bgr, p, p2, CV_RGB(0, 255, 255), 1, 8, 0);
                    
                } // end of loop over the 4 edges
                
                // so far we stored the exact line parameters and show the lines in the image
                // now we have to calculate the exact corners
                cv::Point2f corners[4];
                
                for (int i = 0; i < 4; ++i)
                {
                    int j = (i + 1) % 4;
                    double x0, x1, y0, y1, u0, u1, v0, v1;
                    x0 = lineParams[i + 8]; y0 = lineParams[i + 12];
                    x1 = lineParams[j + 8]; y1 = lineParams[j + 12];
                    
                    u0 = lineParams[i]; v0 = lineParams[i + 4];
                    u1 = lineParams[j]; v1 = lineParams[j + 4];
                    
                    // (x|y) = p + s * vec
                    // s = Ds / D (see cramer's rule)
                    // (x|y) = p + (Ds / D) * vec
                    // (x|y) = (p * D / D) + (Ds * vec / D)
                    // (x|y) = (p * D + Ds * vec) / D
                    // (x|y) = a / c;
                    double a = x1 * u0 * v1 - y1 * u0 * u1 - x0 * u1 * v0 + y0 * u0 * u1;
                    double b = -x0 * v0 * v1 + y0 * u0 * v1 + x1 * v0 * v1 - y1 * v0 * u1;
                    double c = v1 * u0 - v0 * u1;
                    
                    if (fabs(c) < 0.001) //lines parallel?
                    {
                        // std::cout << "lines parallel" << std::endl;
                        continue;
                    }
                    
                    a /= c;
                    b /= c;
                    
                    //exact corner
                    corners[i].x = a;
                    corners[i].y = b;
                    cv::Point p;
                    p.x = (int)corners[i].x;
                    p.y = (int)corners[i].y;
                    
                    cv::circle(img_bgr, p, 5, CV_RGB(255, 255, 0), -1);
                } //finished the calculation of the exact corners
                
                cv::Point2f targetCorners[4];
                targetCorners[0].x = -0.5; targetCorners[0].y = -0.5;
                targetCorners[1].x = 5.5; targetCorners[1].y = -0.5;
                targetCorners[2].x = 5.5; targetCorners[2].y = 5.5;
                targetCorners[3].x = -0.5; targetCorners[3].y = 5.5;
                
                //create and calculate the matrix of perspective transform
                cv::Mat projMat(cv::Size(3, 3), CV_32FC1);
                projMat = cv::getPerspectiveTransform(corners, targetCorners);
                
                
                //create image for the marker
                //            markerSize.width  = 6;
                //            markerSize.height = 6;
                cv::Mat iplMarker(cv::Size(6, 6), CV_8UC1);
                
                //change the perspective in the marker image using the previously calculated matrix
                cv::warpPerspective(img_gray, iplMarker, projMat, cv::Size(6, 6));
                cv::threshold(iplMarker, iplMarker, bw_thresh, 255, THRESH_BINARY);
                
                //now we have a B/W image of a supposed Marker
                
                // check if border is black
                int code = 0;
                for (int i = 0; i < 6; ++i)
                {
                    int pixel1 = iplMarker.at<uchar>(0, i); //top
                    int pixel2 = iplMarker.at<uchar>(5, i); //bottom
                    int pixel3 = iplMarker.at<uchar>(i, 0); //left
                    int pixel4 = iplMarker.at<uchar>(i, 5); //right
                    if ((pixel1 > 0) || (pixel2 > 0) || (pixel3 > 0) || (pixel4 > 0))
                    {
                        code = -1;
                        break;
                    }
                }
                
                if (code < 0) {
                    continue;
                }
                
                //copy the BW values into cP
                int cP[4][4];
                for (int i = 0; i < 4; ++i)
                {
                    for (int j = 0; j < 4; ++j)
                    {
                        cP[i][j] = iplMarker.at<uchar>(i + 1, j + 1);
                        cP[i][j] = (cP[i][j] == 0) ? 1 : 0; //if black then 1 else 0
                    }
                }
                
                //save the ID of the marker
                int codes[4];
                codes[0] = codes[1] = codes[2] = codes[3] = 0;
                for (int i = 0; i < 16; i++)
                {
                    int row = i >> 2;
                    int col = i % 4;
                    
                    codes[0] <<= 1;
                    codes[0] |= cP[row][col]; // 0°
                    
                    codes[1] <<= 1;
                    codes[1] |= cP[3 - col][row]; // 90°
                    
                    codes[2] <<= 1;
                    codes[2] |= cP[3 - row][3 - col]; // 180°
                    
                    codes[3] <<= 1;
                    codes[3] |= cP[col][3 - row]; // 270°
                }
                
                if ((codes[0] == 0) || (codes[0] == 0xffff)) {
                    continue;
                }
                
                //account for symmetry
                code = codes[0];
                int angle = 0;
                for (int i = 1; i < 4; ++i)
                {
                    if (codes[i] < code)
                    {
                        code = codes[i];
                        angle = i;
                    }
                }
                
                //printf("Found: %04x", code);
                //if (code == markerID) printf ("***");
                //printf("\n");
                //markers++;
                //MarkerID.push_back(code);
                
                if (isFirstMarker)
                {
                    //cv::imshow(kWinName4, iplMarker);
                    isFirstMarker = false;
                }
                
                //correct the order of the corners
                if (angle != 0)
                {
                    cv::Point2f corrected_corners[4];
                    for (int i = 0; i < 4; i++)    corrected_corners[(i + angle) % 4] = corners[i];
                    for (int i = 0; i < 4; i++)    corners[i] = corrected_corners[i];
                }
                
                /*
                 // transfer screen coords to camera coords
                 for (int i = 0; i < 4; i++)
                 {
                 corners[i].x -= img_bgr.cols * 0.5; //here you have to use your own camera resolution (x) * 0.5
                 corners[i].y = -corners[i].y + img_bgr.rows * 0.5; //here you have to use your own camera resolution (y) * 0.5
                 }
                 
                 
                 OldOCV_estimateSquarePose(resultMatrix, (cv::Point2f*)corners, kMarkerSize);
                 
                 //resultcode = code;
                 /*
                 //this part is only for printing
                 for (int i = 0; i < 4; ++i) {
                 for (int j = 0; j < 4; ++j) {
                 std::cout << std::setw(6);
                 std::cout << std::setprecision(4);
                 std::cout << resultMatrix[4 * i + j] << " ";
                 }
                 std::cout << "\n";
                 }
                 std::cout << "\n";
                 float x, y, z;
                 x = resultMatrix[3];
                 y = resultMatrix[7];
                 z = resultMatrix[11];
                 std::cout << "length: " << sqrt(x * x + y * y + z * z) << "\n";
                 std::cout << "\n";
                 */
                /*
                 MarkerID.push_back(code);
                 Mat Pmat = (Mat_<double>(4, 4) <<
                 resultMatrix[0], resultMatrix[1], resultMatrix[2], resultMatrix[3],
                 resultMatrix[4], resultMatrix[5], resultMatrix[6], resultMatrix[7],
                 resultMatrix[8], resultMatrix[9], resultMatrix[10], resultMatrix[11],
                 resultMatrix[12], resultMatrix[13], resultMatrix[14], resultMatrix[15]
                 );
                 MarkerPmat.push_back(Pmat);
                 */
                
                /*--- NEW FOR PnP begin ---*/
                int max_d = cv::max(img_bgr.rows, img_bgr.cols);
                int cx = img_bgr.cols / 2.0;
                int cy = img_bgr.rows / 2.0;
                float fx = max_d;
                float fy = max_d;
                Point imgCtr(cx, cy);
                
                _Kmat = (Mat_<double>(3, 3) <<
                         fx, 0., cx,
                         0., fy, cy,
                         0., 0., 1.);
                
                Mat distCoeffs = Mat(1, 4, CV_64FC1, { 0.0,0.0,0.0,0.0 }); // no distortions
                
                double pos0 = kMarkerSize / 2.0f; // half the marker size
                Point3f mcLL(-pos0, -pos0, 0.0); // lower left
                Point3f mcLR(pos0, -pos0, 0.0); // lower right
                Point3f mcUR(pos0, pos0, 0.0); // upper right
                Point3f mcUL(-pos0, pos0, 0.0); // upper left
                //            vector<Point3f> modelMarkerCorners{ mcLL, mcLR, mcUR, mcUL };   // wrong orientation!!! this is COUNTER-CLOCKWISE
                vector<Point3f> modelMarkerCorners{ mcLL, mcUL, mcUR, mcLR };   // this is CLOCKWISE
                
                vector<Point2f> imgMarkerCorners =
                { corners[0], corners[1], corners[2], corners[3] };
                
                Mat raux, taux;
                solvePnP(modelMarkerCorners, imgMarkerCorners, _Kmat, distCoeffs, raux, taux, false);
                
                Mat Rvec;
                raux.convertTo(Rvec, CV_32F);
                Mat_<float> Rmat(3, 3);
                Rodrigues(Rvec, Rmat);
                
                Mat_<float> Tvec;
                taux.convertTo(Tvec, CV_32F);
                Mat Pmat = (Mat_<double>(4, 4) <<
                            Rmat.at<float>(0, 0), Rmat.at<float>(0, 1), Rmat.at<float>(0, 2), Tvec.at<float>(0),
                            Rmat.at<float>(1, 0), Rmat.at<float>(1, 1), Rmat.at<float>(1, 2), Tvec.at<float>(1),
                            Rmat.at<float>(2, 0), Rmat.at<float>(2, 1), Rmat.at<float>(2, 2), Tvec.at<float>(2),
                            0.0f, 0.0f, 0.0f, 1.0f
                            );
                /*--- NEW FOR PnP end ---*/
                
                MarkerID.push_back(code);
                MarkerPmat.push_back(Pmat);
                
            } // end of loop over all contours
            
            /*
             std::cout << "\n----------- \n\n";
             printf("Result: %04x\n", resultcode);
             for (int i = 0; i < 4; ++i) {
             for (int j = 0; j < 4; ++j) {
             std::cout << std::setw(6);
             std::cout << std::setprecision(4);
             std::cout << resultMatrix[4 * i + j] << " ";
             }
             std::cout << "\n";
             }
             std::cout << "\n----------- \n\n";
             */
            
            //cv::imshow(kWinName1, img_bgr);
            //cv::imshow(kWinName2, img_mono);
            
            
            isFirstStripe = true;
            
            isFirstMarker = true;
            
            
            //cvClearMemStorage(memStorage);
        } // end of main loop
        
        //cvClearMemStorage(memStorage);
        
        int key = waitKey(10);
        if (key == 27) exit(0);
    }
        return MarkerID.size();
}
