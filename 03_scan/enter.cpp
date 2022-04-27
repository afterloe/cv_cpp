#include "opencv2/opencv.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

extern cv::Mat resize_img(cv::Mat, int);
extern cv::Mat pretreatment(cv::Mat);
extern void sortP(vector<cv::Point2f> &, vector<cv::Point2f> &, cv::Size &);

int main()
{
    // cv::Mat img = cv::imread("/home/afterloe/Resources/images/scan/receipt.jpg", cv::IMREAD_COLOR);
    cv::Mat img = cv::imread("/home/afterloe/Resources/images/scan/page.jpg", cv::IMREAD_COLOR);
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

    cv::Mat tmp = img.clone();
    // cv::Scalar COLOR_RED(255, 0, 0);

    // for (int i = 0; i < 5; i++)
    // {
    //     cv::drawContours(tmp, contours, i, COLOR_RED, cv::LINE_4);
    //     cv::imshow("img", tmp);
    //     cv::waitKey(0);
    // }

    int idx = 0;
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

    // cv::Scalar COLOR_RED(255, 0, 0);
    // cv::drawContours(tmp, contours, idx, COLOR_RED, cv::LINE_4);

    cv::Mat roi = img(cv::boundingRect(contours[idx]));
    cv::Size img_size = cv::Size(tmp.cols, tmp.rows);

    vector<cv::Point2f> srcTri;
    sortP(approx, srcTri, img_size);

    // for_each(srcTri.begin(), srcTri.end(), [](cv::Point2f &p)
    //          { cout << "x: " << p.x << " y: " << p.y << endl; });

    // 四点透视变换
    vector<cv::Point2f> dstTri(4);
    dstTri[0].x = 0;
    dstTri[0].y = 0;
    dstTri[1].x = srcTri[1].x - srcTri[0].x;
    dstTri[1].y = 0;
    dstTri[2].x = srcTri[1].x - srcTri[0].x;
    dstTri[2].y = srcTri[2].y - srcTri[1].y;
    dstTri[3].x = 0;
    dstTri[3].y = srcTri[2].y - srcTri[1].y;

    cv::Mat H = cv::findHomography(srcTri, dstTri, cv::RANSAC);
    cv::Mat dstImg;

    cv::imshow("img", tmp);
    cv::imshow("roi", roi);

    cv::warpPerspective(tmp, dstImg, H, cv::Size((srcTri[1].x - srcTri[0].x), (srcTri[2].y - srcTri[1].y)));

    cv::imshow("dst", dstImg);

    cv::waitKey(0);

    cv::destroyAllWindows();

    return EXIT_SUCCESS;
}

cv::Mat pretreatment(cv::Mat src)
{
    cv::Mat gray;
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(3, 3), 0);
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

void sortP(vector<cv::Point2f> &v, vector<cv::Point2f> &d, cv::Size &coordinate)
{
    // cv::Point2f *arr = (cv::Point2f *)malloc(sizeof(cv::Point2f) * 4);
    cv::Point2f arr[4];
    for (vector<cv::Point2f>::iterator it = v.begin(); it < v.end(); it++)
    {
        if (it->x == 0)
        {
            continue;
        }
        if (it->x < coordinate.width / 2)
        {
            if (it->y > coordinate.height / 2)
            {
                arr[3] = *it;
            }
            else
            {
                arr[0] = *it;
            }
        }
        else
        {
            if (it->y > coordinate.height / 2)
            {
                arr[2] = *it;
            }
            else
            {
                arr[1] = *it;
            }
        }
    }

    d.clear();

    for (int idx = 0; idx < 4; idx++)
    {
        d.push_back(arr[idx]);
    }

    // free(arr);
    // arr = NULL;
}