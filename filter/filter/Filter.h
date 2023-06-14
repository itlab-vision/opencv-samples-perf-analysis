#include <iostream>
#include <string> 

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>


using namespace std;
using namespace cv;


class Filter
{
public:
	void accuracy_experiment(Mat image);
	void perfomance_experiment();
	Mat create_kernel(int kernel_size);
};
