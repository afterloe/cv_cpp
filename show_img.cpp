#include "opencv2/opencv.hpp"

#include <stdlib.h>
#include <stdio.h>

#define WIN_NAME "view"

int main(int argc, char ** args)
{
    printf("open and show img. \n");
    if (1 == argc) {
        perror("using show_img.app image_of_path");
        return -1;
    }
    args++;
    cv::Mat image = cv::imread(*args, cv::IMREAD_COLOR);
    if (image.empty())
    {
        printf("can't read any byte from %s\n", *args);
        return -1;
    }

    cv::namedWindow(WIN_NAME, cv::WINDOW_AUTOSIZE);
    cv::imshow(WIN_NAME, image);
    cv::waitKey(0);
    cv::destroyAllWindows();

    image.~Mat();
    return 0;
}
