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

	Filter filter = Filter();
	filter.accuracy_experiment(image);
}

