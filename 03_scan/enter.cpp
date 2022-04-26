#include "opencv2/opencv.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

extern cv::Mat resize_img(cv::Mat, int);
extern cv::Mat pretreatment(cv::Mat);

typedef cv::Mat IplImage;

int main()
{
    cv::Mat img = cv::imread("/home/afterloe/Resources/images/scan/receipt.jpg", cv::IMREAD_COLOR);
    if (img.empty())
    {
        printf("读取图片失败. \n");
        return EXIT_FAILURE;
    }
    img = resize_img(img, 1024);
    cv::Mat binary = pretreatment(img);

    vector<vector<cv::Point>> contours;
    cv::findContours(binary, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    sort(contours.begin(), contours.end(), [](vector<cv::Point> one, vector<cv::Point> two) -> bool
         { return cv::contourArea(one) > cv::contourArea(two); });

    IplImage tmp = img.clone();
    int idx = 0;
    // 右上 左上 右下 左下
    vector<cv::Point2f> approx;
    for (int i = 0; i < 5; i++)
    {
        vector<cv::Point> contour = contours[i];
        double peri = cv::arcLength(contour, true);
        cv::approxPolyDP(contour, approx, 0.02 * peri, true);
        if (approx.size() == 4)
        {
            idx = i;
            break;
        }
    }

    for_each(approx.begin(), approx.end(), [](cv::Point2f &p)
             { cout << "x:" << p.x << " y: " << p.y << endl; });

    // cv::Scalar COLOR_RED(255, 0, 0);
    // cv::drawContours(tmp, contours, idx, COLOR_RED, cv::LINE_4);

    cv::Mat roi = img(cv::boundingRect(contours[idx]));

    // 四点透视变换
    vector<cv::Point2f> dstTri(4);
    dstTri[0].x = 0;
    dstTri[0].y = 0;
    dstTri[1].x = approx[0].x - approx[1].x;
    dstTri[1].y = 0;
    dstTri[2].x = approx[0].x - approx[1].x;
    dstTri[2].y = approx[2].y - approx[0].y;
    dstTri[3].x = 0;
    dstTri[3].y = approx[2].y - approx[0].y;

    cv::Mat H = cv::findHomography(approx, dstTri);
    cv::Mat dstImg;
    cv::Size four_size = cv::Size(roi.cols, roi.rows);
    cv::warpPerspective(tmp, dstImg, H, four_size);

    cv::imshow("img", tmp);
    cv::imshow("roi", roi);
    cv::imshow("dst", dstImg);
    cv::waitKey(0);

    cv::destroyAllWindows();

    return EXIT_SUCCESS;
}

cv::Mat pretreatment(cv::Mat src)
{
    cv::Mat gray;
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0);
    cv::Canny(gray, gray, 75, 200);

    return gray;
}

cv::Mat resize_img(cv::Mat src, int width)
{
    cv::Mat tmp;
    float r = (float)width / src.cols;
    cv::resize(src, tmp, cv::Size(width, int(src.rows * r)), 0, 0, cv::INTER_AREA);
    return tmp;
}