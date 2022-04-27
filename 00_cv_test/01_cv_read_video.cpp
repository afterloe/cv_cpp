#include <iostream>

#include "opencv2/opencv.hpp"

using namespace std;

int main()
{
    cv::VideoCapture capture;
    cv::Mat frame;

    int device_id = 0;
    capture.open(device_id);

    if (!capture.isOpened())
    {
        cerr<< "Error! can't read any video is " << device_id << endl;
        return EXIT_FAILURE;
    }

    char key;
    for(;;)
    {
        capture.read(frame);
        if (frame.empty())
        {
            cout << "video is end." << endl;
            break;
        }

        cv::imshow("windows", frame);
        if ('q' == (key = cv::waitKey(5)))
        {
            cout << "enter q to leave." << endl;
            break;
        }
    }

    return EXIT_SUCCESS;
}