#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

#define THRESHOLD 80
#define PI 3.1415926
using namespace cv;
using namespace std;

int main()
{
    VideoCapture capture(0);
    while (true)
    {
        Mat srcImage;
        capture >> srcImage;
        // Mat srcImage = imread("1.jpg");
        Mat dstImage;
        resize(srcImage, dstImage, Size(800, 600), 0, 0, 3);

        Mat grayImage;
        cvtColor(dstImage, grayImage, CV_BGR2GRAY);
        Mat binaryImage;
        threshold(grayImage, binaryImage, THRESHOLD, 255, CV_THRESH_BINARY);
        imshow("binaryImage", binaryImage);
        int rowNum = binaryImage.rows;
        int colNum = binaryImage.cols;
        int blackPixel[rowNum];
        int levelblackPixel[rowNum];
        bool levelLineFlag = false;
        int lineAxis[rowNum];
        bool lineFlag = false;
        vector<Point2f> points;
        Vec4f verticalLine;
        points.clear();
        for (int row = rowNum - 1; row >= 30; row--)
        {
            blackPixel[row] = 0;
            levelblackPixel[row] = 0;
            for (int col = 0; col < colNum - 1; col++)
            {
                int pixelValueNow = binaryImage.at<uchar>(row, col);
                int pixelValueNext = binaryImage.at<uchar>(row, col + 1);
                
                if (pixelValueNow == 0)
                {
                    blackPixel[row]++;
                    levelblackPixel[row]++;
                    if (blackPixel[row] > 100 && pixelValueNext == 255)
                    {
                        lineFlag = true;
                        //cout << "col" << col << "black" << blackPixel[row] << endl;
                        lineAxis[row] = (col + col - blackPixel[row]) / 2;
                        points.push_back(Point2f(lineAxis[row], row));
                        //cout << lineAxis << endl;
                        break;
                    }
                    if (row > 300 && levelblackPixel[row] > 750 && levelblackPixel[row - 1] > 750 && levelblackPixel[row - 2] < 300)
                    {
                        levelLineFlag = true;
                    }
                }
                else
                {
                    lineFlag = false;
                    blackPixel[row] = 0;
                }
            }
        }
        int intercept = 0;
        for (int i = rowNum - 1; i > rowNum - 21; i--)
        {
            intercept = lineAxis[i] + intercept;
        }
        intercept = intercept / 20;
        if (points.size() > 5)
        {
            fitLine(Mat(points), verticalLine, CV_DIST_L1, 0, 0.01, 0.01);
        }

        double k = verticalLine[1] / verticalLine[0];
        double step = 100;
        cv::line(dstImage, cvPoint(verticalLine[2] - step, verticalLine[3] - k * step), cvPoint(verticalLine[2] + step, k * step + verticalLine[3]), Scalar(0, 0, 255), 5);
        imshow("dstImage", dstImage);

        double cos_theta = verticalLine[0];
        double sin_theta = verticalLine[1];
        double x0 = verticalLine[2], y0 = verticalLine[3];

        double phi = atan2(sin_theta, cos_theta) + PI / 2.0;
        double rho = y0 * cos_theta - x0 * sin_theta;
        if (phi > PI / 2 and phi < PI)
        {
            phi = phi - PI;
        }

        cout << "angle: phi = " << phi / PI * 180 << endl;
        cout << "distance: rho = " << intercept << endl;
        cout << "levelLIne: " << (int)levelLineFlag << endl;
        waitKey(5);
    }

    waitKey();
    return 0;
}