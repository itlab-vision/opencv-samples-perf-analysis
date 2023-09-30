#include <iostream>
#include <fstream>
#include <string>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

const char* helper =
"./resize <image_path> <width> <height> <interpolation> <output_path>\n\
\t<image_path> is a file name contained the source image, must be 3 - channel, RGB - image.\n\
\t<width> is a resulting image width.\n\
\t<height> is a resulting image height.\n\
\t<interpolation> is a interpolation algorithm:\n\
\t\t 0 = INTER_NEAREST, 1 = INTER_LINEAR, 2 = INTER_CUBIC, 3 = INTER_AREA, 4 = INTER_LANCZOS4,\n\
\t\t 5 = INTER_LINEAR_EXACT, 6 = INTER_NEAREST_EXACT, 7 = INTER_MAX, 8 = WARP_FILL_OUTLIERS,\n\
\t\t 16 = WARP_INVERSE_MAP.\n\
\t<output_path> is a output file name, if not set then result will not be saved.\n\
";

int proccesArgument(int argc, char* argv[], string& image_path, int& width, int& height,
                    int& interpolation, string& output_path);

double resize(const Mat& image, Mat& dst, const Size& size, int interpolation,
              const string& output_name);


int main(int argc, char* argv[])
{
    string image_path, output_path;
    int width, height, interpolation;

    if (proccesArgument(argc, argv, image_path, width, height, interpolation, output_path) != 0)
    {
        cout << helper << endl;
        return 1;
    }

    Size size(width, height);

    Mat image = imread(image_path, IMREAD_COLOR);
    Mat dst;

    double elapsed_seconds = resize(image, dst, size, interpolation, output_path);
    if (!output_path.empty())
        imwrite(output_path, dst);

    std::cout << "Elapsed time in seconds: " << std::fixed << elapsed_seconds << "\n";
    return 0;
}

int proccesArgument(int argc, char* argv[], string& image_path, int& x, int& y,
                    int& interpolation, string& output_path)
{
    if (argc < 5)
    {
        return 1;
    }

    image_path = argv[1];
    x = atoi(argv[2]);
    y = atoi(argv[3]);
    interpolation = atoi(argv[4]);
    output_path = "";
    if (argc >= 6 && argv[5] != nullptr)
    {
        output_path = argv[5];
    }

    return 0;
}

double resize(const Mat& image, Mat& dst, const Size& size, int interpolation,
              const string& output_name)
{
    auto start = std::chrono::steady_clock::now();
    resize(image, dst, size, 0, 0, interpolation);
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    return elapsed_seconds.count();
}
