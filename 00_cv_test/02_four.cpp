#include<opencv2/opencv.hpp>
using namespace cv;
using namespace std;
 
struct callbackP
{
	Mat src;
	int clickTimes = 0;        //在图像上单击次数
	vector<Point2f> srcTri;
};

Mat resize_img(cv::Mat src, int width)
{
    cv::Mat tmp;
    float r = (float)width / src.cols;
    cv::resize(src, tmp, cv::Size(width, int(src.rows * r)), 0, 0, cv::INTER_AREA);
    return tmp;
}
 
void onMouse(int event, int x, int y, int flags, void *utsc)
{
	callbackP cp = *(callbackP*)utsc;  // 先转换类型，再取数据
 
	if (event == EVENT_LBUTTONUP)      //响应鼠标左键事件
	{
		circle((*(callbackP*)utsc).src, Point(x, y), 2, Scalar(0, 0, 255), 2);  //标记选中点
		imshow("wait ", (*(callbackP*)utsc).src);
		(*(callbackP*)utsc).srcTri.push_back(Point2f(x, y));
		cout << "x:" << x << " " << "y:" << y << endl;
		(*(callbackP*)utsc).clickTimes++;
 
		if ((*(callbackP*)utsc).clickTimes == 4)
		{
			cout << "按任意键继续！" << endl;
		}
	}
}
 
int main(int argc, char *argv[])
{
	vector<Point2f> dstTri(4);
	Mat dst;
	callbackP utsc;
    
    Mat src = imread("/home/afterloe/Resources/images/scan/receipt.jpg");
	utsc.src = resize_img(src, 1024);
	namedWindow("src", WINDOW_AUTOSIZE);
	imshow("src", utsc.src);
	cout << "从需要透视变换区域的左上角开始，顺时针依次点矩形的四个角！" << endl;
	setMouseCallback("src", onMouse, (void*)&utsc);  //类型转换
	waitKey();
 
	if (utsc.clickTimes == 4)
	{
		dstTri[0].x = 0;
		dstTri[0].y = 0;
		dstTri[1].x = utsc.srcTri[1].x - utsc.srcTri[0].x;
		dstTri[1].y = 0;
		dstTri[2].x = utsc.srcTri[1].x - utsc.srcTri[0].x;
		dstTri[2].y = utsc.srcTri[2].y - utsc.srcTri[1].y;
		dstTri[3].x = 0;
		dstTri[3].y = utsc.srcTri[2].y - utsc.srcTri[1].y;
 
		//计算透视矩阵
		Mat M = findHomography(utsc.srcTri, dstTri, RANSAC);
		//图像透视变换
		warpPerspective(utsc.src, dst, M, Size((utsc.srcTri[1].x - utsc.srcTri[0].x), (utsc.srcTri[2].y - utsc.srcTri[1].y)));
		imshow("output", dst);
		imwrite("3p.jpg", dst);
		cout << "透视变换矩阵："<< M << endl;
		waitKey();
	}
	else {
		cout << "需要从左上角开始，顺时针依次点矩形的四个角！" << endl;
		cout << "现在点击了" <<utsc.clickTimes << "次" <<endl;
	}
 
	return 0;
}