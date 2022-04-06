#include "opencv2/opencv.hpp"

#include <iostream>
#include <math.h>
#include <stdlib.h>

extern void help(const char **);

const char *keys = {
    "{help h usage ? |     | print this message}"
    "{@image | ./credit_card_01.png | need ocr image}"
    "{@template | /home/afterloe/Projects/cv_cpp/02_template_matching_ocr/images/ocr_a_reference.png | scan img template }"};

int main(int argc, const char **argv)
{
    cv::CommandLineParser parser(argc, argv, keys);
    if (parser.has("help"))
    {
        help(argv);
        return EXIT_SUCCESS;
    }
    std::string file_name = parser.get<std::string>("@image");

    std::cout << "[INFO]: load image from " << file_name << std::endl;

    cv::Mat tmp = cv::imread(parser.get<std::string>("@template"), cv::IMREAD_COLOR);

    if (tmp.empty())
    {
        perror("[ERROR]");
        printf("[ERROR]: 未加载到模板。");
        return EXIT_FAILURE;
    }

    cv::cvtColor(tmp, tmp, cv::COLOR_BGR2GRAY);
    cv::threshold(tmp, tmp, 10, 255, cv::THRESH_BINARY_INV);
    cv::Mat _t;
    tmp.copyTo(_t);
    std::vector<std::vector<cv::Point>> _contours;
    cv::findContours(_t, _contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    

    cv::imshow("threshold", tmp);


    cv::Mat image_src = cv::imread(file_name, cv::IMREAD_COLOR);
    if (image_src.empty())
    {
        perror("[ERROR]");
        printf("[ERROR]: 图片未找到。");
        return EXIT_FAILURE;
    }
    cv::imshow("src", image_src);
    
    
    cv::waitKey(0);
    cv::destroyAllWindows();

    return EXIT_SUCCESS;
}

void help(const char **argv)
{
    printf("\nopenCV 模板匹配 演示\n"
           "调用方式:\n"
           "          %s [image_name -- default is ./images] \n\n",
           argv[0]);
}

