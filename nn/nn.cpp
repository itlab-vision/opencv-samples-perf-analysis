#include <iostream>
#include <fstream>
#include <regex>
#include <math.h>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

using namespace std;
using namespace cv;
using namespace dnn;
using namespace chrono;

const char* helper =
"./nn <model_file> <model_config> <batch_size> <dataset_path> <labels_file>\n\
\t<model_file> is a file with model.\n\
\t<model_config> is a file config.\n\
\t<batch_size> is a batch size.\n\
\t<dataset_path> is a dataset path.\n\
\t<output_file> is a file in which prediction will be saved (.yml format).\n\
";

int proccesArgument(int argc, char* argv[], string& model_file,
                    string& model_config, string& dataset_path,
                    int& batch_size, string& output_file);

Net load_caffe_model(string model_file, string model_config);

void load_dataset(string dataset_path, vector<Mat>& images);

vector<Mat> slice(vector<Mat>& v, int m, int n);

double convert_images_to_blob(const vector<Mat>& images, Mat& blob);

double inference(Net& model, const Mat& blob, Mat& outputs);

int main(int argc, char* argv[]) {

    string model_file, model_config, dataset_path, output_file;
    int batch_size;

    if (proccesArgument(argc, argv, model_file, model_config, dataset_path, batch_size, output_file) != 0)
    {
        cout << helper << endl;
        return 1;
    }

    vector<Mat> images;
    Mat outputs;

    double convert_time = 0.0;
    double inference_time = 0.0;

    Net model = load_caffe_model(model_file, model_config);
    load_dataset(dataset_path, images);

    int iter_count = ceil((float)images.size() / batch_size);

    for (int i = 0; i < iter_count; i++)
    {
        Mat blob, output;

        int begin = i * batch_size;
        int end = (i + 1) * batch_size;
        end = min(end, (int)images.size());

        vector<Mat> images_batch = slice(images, begin, end);

        convert_time += convert_images_to_blob(images_batch, blob);
        inference_time += inference(model, blob, output);

        if (outputs.empty())
            outputs = output.clone();
        else
            vconcat(outputs, output, outputs);
    }

    cv::FileStorage fs(output_file, cv::FileStorage::WRITE);
    fs << "outputs" << outputs;
    fs.release();

    std::cout << "convert time: " << convert_time << "s\n";
    std::cout << "inference time: " << inference_time << "s\n";
    std::cout << "inference time of one picture: " << inference_time / images.size() << "s\n";

    return 0;
}

int proccesArgument(int argc, char* argv[], string& prototxt_file,
                    string& model_file, string& dataset_path,
                    int& batch_size, string& output_file)
{
    if (argc < 6)
    {
        return 1;
    }

    prototxt_file = argv[1];
    model_file = argv[2];
    dataset_path = argv[3];
    batch_size = atoi(argv[4]);
    output_file = argv[5];

    return 0;
}

Net load_caffe_model(string model_file, string model_config)
{
    return readNet(model_file, model_config);
}

void load_dataset(string dataset_path, vector<Mat>& images)
{
    cv::String path(dataset_path);

    vector<cv::String> fn;
    vector<cv::Mat> data;
    cv::glob(path, fn, true);

    for (size_t k = 0; k < fn.size(); ++k)
    {
        cv::Mat im = cv::imread(fn[k]);
        if (im.empty()) continue;
        images.push_back(im);
    }
}

vector<Mat> slice(vector<Mat>& v, int m, int n)
{
    vector<Mat> vec;
    for (int i = m; i < n; i++)
    {
        vec.push_back(v[i]);
    }
    return vec;
}

double convert_images_to_blob(const vector<Mat>& images, Mat& blob)
{
    auto start = std::chrono::steady_clock::now();
    blob = blobFromImages(images, 1, Size(224, 224), Scalar(104, 117, 123));
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> convert_time = end - start;
    return convert_time.count();
}

double inference(Net& model, const Mat& blob, Mat& outputs)
{
    auto start = std::chrono::steady_clock::now();
    model.setInput(blob);
    outputs = model.forward();
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> convert_time = end - start;
    return convert_time.count();
}