#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

const char* helper = 
"./diff_images <image1> <image2>\n\
\t<image1> is the first image.\n\
\t<image2> is the second image.\n\
";

int proccesArgument(int argc, char* argv[], string& image_file1, string& image_file2)
{
    if (argc < 3)
    {
        return 1;
    }
    image_file1 = argv[1];
    image_file2 = argv[2];

    return 0;
}

int count_differences(const Mat& image1, const Mat& image2) 
{
    int number_different_pixels = 0;

    for (int i = 0; i < image1.rows; i++)
        for (int j = 0; j < image1.cols; j++)
            if (image1.at<cv::Vec3b>(i, j) != image2.at<cv::Vec3b>(i, j))
                number_different_pixels++;

    return number_different_pixels;
}


int main(int argc, char** argv)
{
    string image_file1, image_file2;

    if (proccesArgument(argc, argv, image_file1, image_file2) != 0)
    {
        cout << helper << endl;
        return 1;
    }

    Mat image1 = imread(image_file1, IMREAD_COLOR);
    Mat image2 = imread(image_file2, IMREAD_COLOR);

    if (image1.size() != image2.size())
    {
        cout << "images of different sizes" << endl;
        cout << " image1 size: " << image1.size() << endl;
        cout << " image2 size: " << image2.size() << endl;
        return 1;
    }
    int number_different_pixels = count_differences(image1, image2);
    cout << "number of differences: " << number_different_pixels << endl;

    return 0;
}
