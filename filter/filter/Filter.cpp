#include "Filter.h"


void Filter::accuracy_experiment(Mat image)
{
	Mat dst;
	string name;
	string dir = "result/";

	int ddepth = -1;
	Point anchor = Point(-1, -1);
	int delta = 0;

	std::vector<int> borders = { BORDER_CONSTANT, BORDER_REPLICATE, BORDER_REFLECT };

	for (int kernel_size = 3; kernel_size <= 13; kernel_size +=2)
	{
		Mat kernel = create_kernel(kernel_size);
		for (int border_type : borders)
		{
			filter2D(image, dst, ddepth, kernel, anchor, delta, border_type);
			name = dir + "kernel_size_" + std::to_string(kernel_size) + "_";
			name += "border_type_" + std::to_string(border_type) + ".jpg";
			imwrite(name, dst);
		}
	}
}


Mat Filter::create_kernel(int kernel_size)
{
	return Mat::ones(kernel_size, kernel_size, CV_32F) / (float)(kernel_size * kernel_size);
}

void Filter::perfomance_experiment()
{

}
