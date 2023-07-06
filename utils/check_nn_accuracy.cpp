#include <iostream>
#include <fstream>
#include <string>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

const char* helper =
"./check_nn_accuracy <predictions> <labels>\n\
\t<predictions> is a file contained predicted class identifiers.\n\
\t<labels> is a file contained labeled class identifiers.\n\
";

void metrics_calculation(Mat outputs, Mat labels);


int main(int argc, char** argv)
{
    Mat outputs, labels;
    fstream label_file;

    if (argc < 3)
    {
        cout << helper;
        return 1;
    }
    FileStorage fs_read(argv[1], FileStorage::READ);
    fs_read["outputs"] >> outputs;
    fs_read.release();

    label_file.open(argv[2], ios::in);
    if (label_file.is_open()) {
        string tp;
        while (getline(label_file, tp))
            labels.push_back(stoi(tp));
        label_file.close();
    }

    metrics_calculation(outputs, labels);
    return 0;
}

void metrics_calculation(Mat outputs, Mat labels)
{
    int top1_accuracy = 0;
    int top5_accuracy = 0;
    for (int i = 0; i < labels.rows; i++)
    {
        double maxProb;
        Point pos;

        Mat row = outputs.row(i).clone();

        minMaxLoc(row, nullptr, &maxProb, nullptr, &pos);
        if (pos.x == labels.row(i).at<int>(0)) 
            top1_accuracy++;


        Mat B;
        sortIdx(row, B, SORT_EVERY_ROW + SORT_DESCENDING);
        for (int j = 0; j < 5; j++)
            if (B.at<int>(0, j) == labels.row(i).at<int>(0))
            {
                top5_accuracy++;
                break;
            }
    }
    cout << "top 1 accuracy: " << (float)top1_accuracy / labels.rows << endl;
    cout << "top 5 accuracy: " << (float)top5_accuracy / labels.rows << endl;
}