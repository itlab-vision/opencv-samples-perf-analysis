#include <iostream>
#include <fstream>
#include <string>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

const char* helper =
"./filter <image_path> <kernel_path> <x> <y> <border_type> <output_path>\
\t<image_path> is a file name contained the source image (3-channel, RGB)\n\
\t<kernel_path> is a file name in the txt-format that stores the kernel\n\
\t<x> is an anchor x coordinate\n\
\t<y> is an anchor x coordinate\n\
\t<border_type> is a border type: 0 = BORDER_CONSTANT, 1 = BORDER_REPLICATE, 2 = BORDER_REFLECT.\n\
\t<output_path> is an output file name.\n\
";

int proccesArgument(int argc, char* argv[], string& image_path,
                    string& kernel_path, int& x, int& y, int& border_type,
                    string& output_path);

Mat parse_kernel_from_txt(const string& kernel_path);

double filter(const Mat& image, Mat& dst, const Mat& kernel, const Point& anchor,
           int border_type, const string& output_name);


int main(int argc, char* argv[])
{
    string image_path, kernel_path, output_path;
    int x, y, border_type;
    if (proccesArgument(argc, argv, image_path, kernel_path, x, y,
            border_type, output_path) != 0)
    {
        cout << helper << endl;
        return 1;
    }

    Point anchor(x, y);
    Mat image = imread(image_path, IMREAD_COLOR);
    Mat dst;
    Mat kernel = parse_kernel_from_txt(kernel_path);

    double elapsed_seconds = filter(image, dst, kernel, anchor,
                                 border_type, output_path);

    imwrite(output_path, dst);
    std::cout << "elapsed time: " << elapsed_seconds << "s\n";

    return 0;
}

int proccesArgument(int argc, char* argv[], string& image_path,
                    string& kernel_path, int& x, int& y, int& border_type,
                    string& output_path)
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

Mat parse_kernel_from_txt(const string& kernel_path)
{
    fstream newfile;
    std::stringstream stream("");

    string file_line;
    float value;
    int rows, cols;

    Mat kernel;

    newfile.open(kernel_path, ios::in);
    if (newfile.is_open())
    {
        getline(newfile, file_line);
        stream.clear();
        stream.str(file_line);
        stream >> value;
        rows = (int)value;

        getline(newfile, file_line);
        stream.clear();
        stream.str(file_line);
        stream >> value;
        cols = (int)value;

        kernel.create(rows, cols, CV_32F);
        for (int i = 0; i < rows; i++)
        {
            getline(newfile, file_line);
            stream.clear();
            stream.str(file_line);
            for (int j = 0; j < cols; j++)
            {
                stream >> value;
                kernel.at<float>(i, j) = value;
            }
        }
        newfile.close();
    }
    return kernel;
}

double filter(const Mat& image, Mat& dst, const Mat& kernel, const Point& anchor,
           int border_type, const string& output_name)
{
    auto start = std::chrono::steady_clock::now();
    filter2D(image, dst, -1, kernel, anchor, 0, border_type);
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    return elapsed_seconds.count();
}