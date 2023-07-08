#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class BaseReader
{
public:
    virtual void load_train_data(const string& data_dir, vector<Mat>& images, Mat& labels) = 0;
    virtual void load_test_data(const string& data_dir, vector<Mat>& images, Mat& labels) = 0;
    virtual void load_test_labels(const string& data_dir, Mat& labels) = 0;
};

