#include <iostream>
#include <fstream>
#include <string>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

const char* helper =
"./erosion <image_path> <erosion_size> <x> <y> <iterations> <border_type> <output_path>\n\
\t<image_path> is a file name contained the source image, must be 3 - channel, RGB - image.\n\
\t<erosion_size> is a erosion size for kernel (kernel size = 2*erosion_size + 1).\n\
\t<x> is an anchor x coordinate.\n\
\t<y> is an anchor y coordinate.\n\
\t<iterations> is a number of times erosion is applied.\n\
\t<border_type> is a border type: 0 = BORDER_CONSTANT, 1 = BORDER_REPLICATE, 2 = BORDER_REFLECT.\n\
\t<output_path> is a output file name, if not set then result will not be saved.\n\
";

int proccesArgument(int argc, char* argv[], string& image_path, int& erosion_size,
                    int &x, int &y, int &iterations, int &border_type, string& output_path);

double erosion(const Mat& gray, Mat& dst, const Mat& kernel, 
               const Point& anchor, int iterations, int border_type);


int main(int argc, char* argv[])
{
    string image_path, output_path;
    int erosion_size, x, y, iterations, border_type;
    if (proccesArgument(argc, argv, image_path, erosion_size, x, y, iterations, border_type, output_path) != 0)
    {
        cout << helper << endl;
        return 1;
    }

    Mat image = imread(image_path, IMREAD_COLOR);
    Mat gray, dst;
    cvtColor(image, gray, COLOR_RGB2GRAY);

    Point anchor(x, y);

    Mat kernel = getStructuringElement(MORPH_RECT,
                                       Size(2 * erosion_size + 1, 2 * erosion_size + 1),
                                       Point(erosion_size, erosion_size));

    double elapsed_seconds = erosion(gray, dst, kernel, anchor, iterations, border_type);
    if (!output_path.empty())
        imwrite(output_path, dst);

    std::cout << "elapsed time: " << elapsed_seconds << "s\n";
    return 0;
}

int proccesArgument(int argc, char* argv[], string& image_path, int& erosion_size,
                    int &x, int &y, int &iterations, int &border_type, string& output_path)
{
    if (argc < 7)
    {
        return 1;
    }

    image_path = argv[1];
    erosion_size = atoi(argv[2]);
    x = atoi(argv[3]);
    y = atoi(argv[4]);
    if ((x < -1) || (x >=  2 * erosion_size + 1))
    {
        cout << "x value must be greater than -1 and less than 10" << 2 * erosion_size + 1 << endl;
        return -1;
    }
    if ((y < -1) || (y >=  2 * erosion_size + 1))
    {
        cout << "y value must be greater than -1 and less than 10" << 2 * erosion_size + 1 << endl;
        return -1;
    }

    iterations = atoi(argv[5]);
    border_type = atoi(argv[6]);
    output_path = "";
    if (argc >= 8 && argv[7] != nullptr)
    {
        output_path = argv[7];
    }

    return 0;
}

double erosion(const Mat& gray, Mat& dst, const Mat& kernel, 
               const Point& anchor, int iterations, int border_type)
{
    auto start = std::chrono::steady_clock::now();
    erode(gray, dst, kernel, anchor, iterations, border_type);
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    return elapsed_seconds.count();
}
