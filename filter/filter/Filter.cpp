#include "Filter.h"


void Filter::experiment(Mat image, Mat kernel, int border_type, string output_name)
{
	Mat dst;

	int ddepth = -1;
	Point anchor = Point(-1, -1);
	int delta = 0;

	auto start = std::chrono::steady_clock::now();
	filter2D(image, dst, ddepth, kernel, anchor, delta, border_type);
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";

	imwrite(output_name, dst);
}
