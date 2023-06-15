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
	void experiment(Mat image, Mat kernel, int border_type, string output_name);
};
