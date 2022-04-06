#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <stdio.h>

#define BUF_SIZE 128
#define WIN_NAME "view"

class Tools
{
public:
    char *INFO(char *, ...);
    char *ERROR(char *, ...);
};

int main(int argc, char **args)
{
    Tools tools = Tools();
    tools.INFO("准备加载视频.");
    if (1 == argc)
    {
        tools.ERROR("using show_video.app video_of_path.");
        return -1;
    }
    args++;
    tools.INFO(*args);
    cv::VideoCapture video_stream;
    video_stream.open(*args, cv::CAP_ANY);
    if (!video_stream.isOpened())
    {
        tools.ERROR("video can't open.");
        return -1;
    }

    tools.INFO("start grabbing");

    int fps = video_stream.get(cv::CAP_PROP_FPS);
    cv::Mat frame;
    cv::namedWindow(WIN_NAME, cv::WINDOW_AUTOSIZE);

    while (1)
    {
        video_stream.read(frame);
        if (frame.empty())
        {
            tools.INFO("video is end.");
            break;
        }
        cv::imshow(WIN_NAME, frame);
        if (cv::waitKey(1000 / fps) == 'q')
        {
            tools.INFO("enter q ... close windows. app will stop");
            break;
        }
    }

    return 0;
}

char *Tools::INFO(char *content, ...)
{
    char *p = (char *)malloc(BUF_SIZE);
    sprintf(p, "[INFO]: %s\n", content);
    printf("%s", p);
    return p;
}

char *Tools::ERROR(char *content, ...)
{
    char *p = (char *)malloc(BUF_SIZE);
    sprintf(p, "[ERROR]: %s\n", content);
    printf("%s", p);
    return p;
}