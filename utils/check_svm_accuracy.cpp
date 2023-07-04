#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

const char* helper =
"./check_svm_accuracy <file>\n\
\t<file> is a file contained labeled and predicted class identifiers\n\
";

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        cout << helper;
        return 1;
    }

    Mat outputs, labels;
    FileStorage fs_read(argv[1], FileStorage::READ);
    fs_read["outputs"] >> outputs;
    fs_read["labels"] >> labels;
    fs_read.release();

    int number_correct = 0;
    for (int i = 0; i < outputs.rows; i++)
    {
        if (outputs.at<int>(0, i) == labels.at<int>(0, i))
            number_correct++;
    }
    cout << "accuracy: " << (float)number_correct / outputs.rows;
    return 0;
}
