#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

#include "CifarReader.h"

using namespace std;
using namespace cv;

const char* helper =
"./check_svm_accuracy <dataset> <predictions> <labels>\n\
\t<dataset> is a string containing one of the supported datasets: <cifar10>.\n\
\t<predictions> is a file contained predicted class identifiers.\n\
\t<labels> is a file contained labeled class identifiers.\n\
";

int main(int argc, char** argv)
{
    Mat outputs, labels;
    const map<string, BaseReader*> readers = {
        {"cifar10", new CifarReader()}
    };

    if (argc < 4)
    {
        cout << helper;
        return 1;
    }

    FileStorage fs_read(argv[2], FileStorage::READ);
    fs_read["outputs"] >> outputs;
    fs_read.release();

    BaseReader* reader = readers.at(argv[1]);
    reader->load_test_labels(argv[3], labels);

    int number_correct = 0;
    for (int i = 0; i < outputs.rows; i++)
    {
        if (outputs.at<int>(0, i) == labels.at<int>(0, i))
            number_correct++;
    }

    cout << "accuracy: " << (float)number_correct / outputs.rows << endl;
    return 0;
}
