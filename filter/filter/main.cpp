#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "Filter.h"

using namespace std;
using namespace cv;

int main()
{
	Mat image = Mat::zeros(500, 500, CV_8UC3);
	circle(image, Point(250, 150), 100, Scalar(0, 255, 128), -100);
	circle(image, Point(350, 150), 100, Scalar(255, 255, 255), -100);

	int kernel_size = 3;
	Mat kernel = Mat::ones(kernel_size, kernel_size, CV_32F) / (float)(kernel_size * kernel_size);

	int border_type = BORDER_CONSTANT;

	Filter filter = Filter();
	filter.experiment(image, kernel, border_type, "result.png");
}
