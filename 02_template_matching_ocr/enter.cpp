#include "opencv2/opencv.hpp"
#include "opencv2/core/hal/interface.h"

#include <iostream>
#include <numeric>

extern void help(const char **);

const char *keys = {
    "{help h usage ? |     | print this message}"
    "{@image    | /home/afterloe/Projects/cv_cpp/02_template_matching_ocr/images/credit_card_01.png  | need ocr image}"
    "{@template | /home/afterloe/Projects/cv_cpp/02_template_matching_ocr/images/ocr_a_reference.png | scan img template }"};

int main(int argc, const char **argv)
{
    cv::CommandLineParser parser(argc, argv, keys);
    if (parser.has("help"))
    {
        help(argv);
        return EXIT_SUCCESS;
    }

    cv::Mat tmpleate_img = cv::imread(parser.get<std::string>("@template"), cv::IMREAD_COLOR);
    if (tmpleate_img.empty())
    {
        perror("[ERROR]");
        printf("[ERROR]: 模板未加载。");
        return EXIT_FAILURE;
    }
    cv::Mat threshed;
    cv::cvtColor(tmpleate_img, threshed, cv::COLOR_BGR2GRAY);
    cv::threshold(threshed, threshed, 10, 255, cv::THRESH_BINARY_INV);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(threshed, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    cv::Scalar COLOR_RED(0, 0, 255);
    std::vector<cv::Rect2f> fragments(contours.size());
    for (int idx = 0; idx < contours.size(); idx++)
    {
        fragments.push_back(cv::boundingRect(contours[idx]));
    }
    std::sort(fragments.begin(), fragments.end(), [](cv::Rect2f a, cv::Rect2f b) -> bool
              { return a.x < b.x; });

    cv::Mat rect_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 3));
    cv::Mat sq_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    std::string file_name = parser.get<std::string>("@image");
    std::cout << "[INFO]: load image from " << file_name << std::endl;

    cv::Mat ocr_image = cv::imread(file_name, cv::IMREAD_COLOR);
    if (ocr_image.empty())
    {
        perror("[ERROR]");
        printf("[ERROR]: 图片未找到。");
        return EXIT_FAILURE;
    }

    float r = 300.0f / ocr_image.cols;
    cv::resize(ocr_image, ocr_image, cv::Size(300, int(ocr_image.rows * r)), 0, 0, cv::INTER_AREA);

    cv::Mat ocr_image_tmp = ocr_image.clone();
    cv::cvtColor(ocr_image, ocr_image_tmp, cv::COLOR_BGR2GRAY);

    cv::Mat ocr_gray_image_tmp = ocr_image_tmp.clone();

    cv::GaussianBlur(ocr_image_tmp, ocr_image_tmp, cv::Size(3, 3), 1);

    cv::morphologyEx(ocr_image_tmp, ocr_image_tmp, cv::MORPH_TOPHAT, rect_kernel);
    cv::morphologyEx(ocr_image_tmp, ocr_image_tmp, cv::MORPH_CLOSE, rect_kernel);
    cv::threshold(ocr_image_tmp, ocr_image_tmp, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    cv::morphologyEx(ocr_image_tmp, ocr_image_tmp, cv::MORPH_CLOSE, sq_kernel);

    std::vector<std::vector<cv::Point>> ocr_contours;
    cv::findContours(ocr_image_tmp, ocr_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    // cv::drawContours(ocr_image, ocr_contours, -1, COLOR_RED, 3, cv::LINE_AA);

    std::vector<cv::Rect2f> ocr_fragments(ocr_contours.size());
    for (int idx = 0; idx < ocr_contours.size(); idx++)
    {
        cv::Rect2f box = cv::boundingRect(ocr_contours[idx]);
        float ar = box.width / float(box.height);
        if (ar < 2.5 || ar > 4.0)
        {
            continue;
        }
        if (box.width < 40 || box.width > 55)
        {
            continue;
        }
        if (box.height < 10 || box.height > 20)
        {
            continue;
        }
        ocr_fragments.push_back(box);
    }

    std::sort(ocr_fragments.begin(), ocr_fragments.end(), [](cv::Rect2f a, cv::Rect2f b) -> bool
              { return a.x < b.x; });
    std::vector<char *> card_num(ocr_fragments.size());

    for (int idx = 0; idx < ocr_fragments.size(); idx++)
    {
        if (ocr_fragments[idx].empty())
        {
            continue;
        }
        cv::Mat nums = ocr_gray_image_tmp(ocr_fragments[idx]);

        cv::threshold(nums, nums, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
        std::vector<std::vector<cv::Point>> nums_contours;
        cv::findContours(nums, nums_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        std::vector<cv::Rect2f> items_ocr_fragments(contours.size());
        for (int i = 0; i < nums_contours.size(); i++)
        {
            items_ocr_fragments.push_back(cv::boundingRect(nums_contours[i]));
        }
        std::sort(items_ocr_fragments.begin(), items_ocr_fragments.end(), [](cv::Rect2f a, cv::Rect2f b) -> bool
                  { return a.x < b.x; });

        for (int i = 0; i < items_ocr_fragments.size(); i++)
        {
            if (items_ocr_fragments[i].empty())
            {
                continue;
            }
            cv::Mat num = nums(items_ocr_fragments[i]);
            cv::resize(num, num, cv::Size(57, 88));
            // double *sources = (double *)malloc(fragments.size());

            std::vector<double> sources(fragments.size());

            cv::Mat result;
            double min_val, max_val;
            cv::Point minLoc, maxLoc;
            for (int j = 0; j < fragments.size(); j++)
            {
                if (fragments[j].empty())
                {
                    continue;
                }
                cv::Mat num_tmp = threshed(fragments[j]);
                cv::resize(num_tmp, num_tmp, cv::Size(57, 88));
                cv::matchTemplate(num, num_tmp, result, cv::TM_CCOEFF);
                cv::minMaxLoc(result, &min_val, &max_val, &minLoc, &maxLoc, cv::Mat());
                // sources[j] = max_val;
                sources.push_back(max_val);
                // printf("%.3f ", max_val);
            }

            int val = -1, index = 0;
            double s = 0.0;
            printf("[");
            for (int j = 0; j < sources.size(); j++)
            {
                if (0 == sources[j])
                {
                    continue;
                }
                printf("%.3f ", sources[j]);

                if (sources[j] > s)
                {
                    s = sources[j];
                    val = index;
                }
                index++;
            }
            printf("]");
            printf("\n");
            printf("%d \n", val);
            char *num_str = (char *)malloc(1);
            sprintf(num_str, "%d", val);
            card_num.push_back(num_str);
            printf("--------------- ----------------- ---------------\n");
        }
    }

    printf("银行卡号： ");
    for (int idx = 0; idx < card_num.size(); idx++)
    {
        if (card_num[idx] != NULL)
        {
            printf("%s", card_num[idx]);
        }
    }
    printf("\n");
    cv::imshow("ocr_image", ocr_image);
    cv::waitKey(0);

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
