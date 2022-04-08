#include "opencv2/opencv.hpp"

#include <iostream>

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

    cv::Mat tmpl = cv::imread(parser.get<std::string>("@template"), cv::IMREAD_COLOR);
    if (tmpl.empty())
    {
        perror("[ERROR]");
        printf("[ERROR]: 模板未加载。");
        return EXIT_FAILURE;
    }
    cv::Mat threshed;
    cv::cvtColor(tmpl, threshed, cv::COLOR_BGR2GRAY);
    cv::threshold(threshed, threshed, 10, 255, cv::THRESH_BINARY_INV);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(threshed, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    cv::Scalar COLOR_RED(0, 0, 255);
    std::vector<cv::Rect2f> fragments(contours.size());
    for (int idx = 0; idx < contours.size(); idx++)
    {
        // cv::rectangle(tmpl, box, color, 2, cv::LINE_4);
        fragments.push_back(cv::boundingRect(contours[idx]));

        // cv::RotatedRect box = cv::minAreaRect(contours[idx]);
        // std::vector<cv::Point2f> box_pts(4);
        // box.points(box_pts.data());

        // float x_ptr, y_ptr, fragment_width, fragment_height;
        // x_ptr = box_pts[0].x;
        // y_ptr = box_pts[0].y;
        // fragment_width = box_pts[1].x - x_ptr;
        // fragment_height = box_pts[2].y - y_ptr;

        // std::cout << box_pts << std::endl;
        // std::cout << x_ptr << " - " << y_ptr << " - " << fragment_width << " - " << fragment_height << " - " << std::endl;
        // std::cout << "------ ------------- -------" << std::endl;

        // for (int j = 0; j < 4; j++)
        // {
        //     std::cout << box_pts[j].x << " | " << box_pts[j].y << std::endl;
        //     std::cout << box_pts[(j + 1) % 4] << std::endl;
        //     std::cout << "------ -------- --------- ----------"<< std::endl;
        //     line(tmpl, box_pts[j], box_pts[(j + 1) % 4], color, 2, cv::LINE_4);
        // }
        // std::cout << box_pts << std::endl;

        // x y width h

        // cv::Mat fragment = tmpl(cv::Rect2f(x_ptr, y_ptr, fragment_width, fragment_height));
        // cv::imshow("f", fragment);
        // cv::waitKey(0);

        // break;
        // fragments.push_back();
    }

    std::sort(fragments.begin(), fragments.end(), [](cv::Rect2f a, cv::Rect2f b) -> bool
              { return a.x < b.x; });

    for (int idx = 0; idx < fragments.size(); idx++)
    {
        if (fragments[idx].empty())
        {
            continue;
        }

        std::cout << fragments[idx] << std::endl;

        cv::imshow("f", tmpl(fragments[idx]));
        cv::waitKey(0);
    }

    // std::string file_name = parser.get<std::string>("@image");
    // std::cout << "[INFO]: load image from " << file_name << std::endl;

    // cv::Mat image_src = cv::imread(file_name, cv::IMREAD_COLOR);
    // if (image_src.empty())
    // {
    //     perror("[ERROR]");
    //     printf("[ERROR]: 图片未找到。");
    //     return EXIT_FAILURE;
    // }
    // cv::imshow("src", image_src);

    cv::destroyAllWindows();

    return EXIT_SUCCESS;
}

void help(const char **argv)
{
    printf("\nopenCV 模板匹配 演示\n"
           "调用方式:\n"
           "          %s -image image path \\\n"
           "            [-template template image path]\n\n",
           argv[0]);
}
