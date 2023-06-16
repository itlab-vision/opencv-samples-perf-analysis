#include <iostream>
#include <fstream>
#include <string>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

const char* helper =
"\t<image_path> - file name contained the source image, must be 3 - channel, RGB - image\n\
\t<kernel_path> - file in txt format that stores the convolution kernel\n\
\t<x> - anchor.x\n\
\t<y> - anchor.y\n\
\t<border_type> - border handling.\n\
\t<output_path> - output file name..\n\
";


int proccesArgument(int argc, char* argv[], string& image_path, string& kernel_path, int& x, int& y, int& border_type, string& output_path);
Mat vector_to_mat(vector<vector<float>> kernel);
Mat parse_kernel_from_txt(string kernel_path);
void filter(Mat& image, Mat& dst, Mat& kernel, Point anchor, int border_type, string output_name);


int main(int argc, char* argv[])
{
	string image_path, kernel_path, output_path;
	int x, y, border_type;
	if (proccesArgument(argc, argv, image_path, kernel_path, x, y, border_type, output_path) != 0)
	{
		cout << helper << endl;
		return 1;
	}

	Point anchor(x, y);

	Mat image = imread(image_path, IMREAD_COLOR);
	Mat dst;
	Mat kernel = parse_kernel_from_txt(kernel_path);

	filter(image, dst, kernel, anchor, border_type, output_path);
	imwrite(output_path, dst);
}

int proccesArgument(int argc, char* argv[], string& image_path, string& kernel_path, int& x, int& y, int& border_type, string& output_path)
{
	if (argc < 7)
	{
		return 1;
	}

	image_path = argv[1];
	kernel_path = argv[2];
	x = atoi(argv[3]);
	y = atoi(argv[4]);
	border_type = atoi(argv[5]);
	output_path = argv[6];

	return 0;
}

Mat vector_to_mat(vector<vector<float>> kernel)
{
	Mat opencv_kernel(kernel.size(), kernel[0].size(), CV_32F);

	for (int i = 0; i < kernel.size(); i++)
	{
		for (int j = 0; j < kernel[i].size(); j++)
		{
			opencv_kernel.at<float>(i, j) = kernel[i][j];
		}
	}
	return opencv_kernel;
}


Mat parse_kernel_from_txt(string kernel_path)
{
	fstream newfile;
	std::stringstream stream("");

	vector<vector<float>> kernel;

	vector<float> row;
	string file_line;
	float value;

	newfile.open(kernel_path, ios::in);
	if (newfile.is_open())
	{
		while (getline(newfile, file_line))
		{
			stream.clear();
			stream.str(file_line);

			while (stream >> value)
				row.push_back(value);

			kernel.push_back(row);
			row.clear();
		}
		newfile.close();
	}

	Mat opencv_kernel = vector_to_mat(kernel);
	return opencv_kernel;
}


void filter(Mat& image, Mat& dst, Mat& kernel, Point anchor, int border_type, string output_name)
{
	auto start = std::chrono::steady_clock::now();

	filter2D(image, dst, -1, kernel, anchor, 0, border_type);

	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
}