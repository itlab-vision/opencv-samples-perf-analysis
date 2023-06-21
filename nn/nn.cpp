#include <iostream>
#include <fstream>
#include <regex>
#include <math.h>
#include <algorithm>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

using namespace std;
using namespace cv;
using namespace dnn;
using namespace chrono;

const char* helper =
"./nn <model_file> <model_config> <dataset_path> <scale_factor> <batch_size> <width> <height>\
<intensity_r> <intensity_g> <intensity_b> <output_file>\n\
\t<model_file> is a file with model.\n\
\t<model_config> is a file config.\n\
\t<dataset_path> is a dataset path.\n\
\t<scale_factor> is a scale factor.\n\
\t<batch_size> is a batch size.\n\
\t<width> is a width of the image that the model receives.\n\
\t<height> is a height of the image that the model receives.\n\
\t<intensity_r> is an average red channel intensity.\n\
\t<intensity_g> is an average green channel intensity.\n\
\t<intensity_b> is an average blue channel intensity.\n\
\t<output_file> is a file in which prediction will be saved (.yml format).\n\
";

int proccesArgument(int argc, char* argv[], string& model_file,
    string& model_config, string& dataset_path,
    double& scale_factor, int& batch_size, int& width, int& height,
    int& intensity_r, int& intensity_g, int& intensity_b,
    string& output_file);

Net load_model(string model_file, string model_config);

void load_batch(vector<String>& fn, int m, int n, vector<Mat>& images);

double convert_images_to_blob(const vector<Mat>& images, Mat& blob,
    double scale_factor, int width, int height,
    int intensity_r, int intensity_g, int intensity_b);

double inference(Net& model, const Mat& blob, Mat& outputs);

double find_median(vector<double> times);

double find_average(vector<double> times);

int main(int argc, char* argv[]) {

    string model_file, model_config, dataset_path, output_file;
    int batch_size, width, height, intensity_r, intensity_g, intensity_b;
    double scale_factor;

    if (proccesArgument(argc, argv, model_file, model_config, dataset_path,
        scale_factor, batch_size, width, height,
        intensity_r, intensity_g, intensity_b,
        output_file) != 0)
    {
        cout << helper << endl;
        return 1;
    }

    cv::String path(dataset_path);

    vector<String> fn;
    vector<Mat> data;

    cv::glob(path, fn, true);

    Mat outputs;

    double total_convert_time = 0.0;
    double total_inference_time = 0.0;

    vector<double> batch_times;

    Net model = load_model(model_file, model_config);

    int iter_count = ceil((float)fn.size() / batch_size);

    for (int i = 0; i < iter_count; i++)
    {
        vector<Mat> images;
        Mat blob, output;

        int begin = i * batch_size;
        int end = (i + 1) * batch_size;
        end = min(end, (int)fn.size());

        load_batch(fn, begin, end, images);

        total_convert_time += convert_images_to_blob(images, blob, scale_factor, width, height,
            intensity_r, intensity_g, intensity_b);
        double inference_time = inference(model, blob, output);
        total_inference_time += inference_time;
        batch_times.push_back(inference_time);

        if (outputs.empty())
            outputs = output.clone();
        else
            vconcat(outputs, output, outputs);
    }

    cv::FileStorage fs(output_file, cv::FileStorage::WRITE);
    fs << "outputs" << outputs;
    fs.release();

    std::cout << "convert time: " << total_convert_time << " s\n";
    std::cout << "inference time: " << total_inference_time << " s\n";
    std::cout << "inference time per image: " << total_inference_time / fn.size() << " s\n";

    double latency = find_median(batch_times);
    double average_time = find_average(batch_times);

    std::cout << "latency: " << latency << " s\n";
    std::cout << "average time per batch: " << average_time << " s\n";

    std::cout << "FPS: " << batch_size / latency << endl;

    return 0;
}

int proccesArgument(int argc, char* argv[], string& model_file,
    string& model_config, string& dataset_path,
    double& scale_factor, int& batch_size, int& width, int& height,
    int& intensity_r, int& intensity_g, int& intensity_b,
    string& output_file)
{
    if (argc < 12)
    {
        return 1;
    }

    model_file = argv[1];
    model_config = argv[2];
    dataset_path = argv[3];

    scale_factor = stod(argv[4]);

    batch_size = atoi(argv[5]);

    width = atoi(argv[6]);
    height = atoi(argv[7]);

    intensity_r = atoi(argv[8]);
    intensity_g = atoi(argv[9]);
    intensity_b = atoi(argv[10]);

    output_file = argv[1];

    return 0;
}

Net load_model(string model_file, string model_config)
{
    return readNet(model_file, model_config);
}

void load_batch(vector<String>& fn, int begin, int end, vector<Mat>& images)
{
    for (size_t i = begin; i < end; i++)
    {
        cv::Mat im = cv::imread(fn[i]);
        if (im.empty()) continue;
        images.push_back(im);
    }
}

double convert_images_to_blob(const vector<Mat>& images, Mat& blob,
    double scale_factor, int width, int height,
    int intensity_r, int intensity_g, int intensity_b)
{
    auto start = std::chrono::steady_clock::now();
    blob = blobFromImages(images, scale_factor, Size(width, height),
        Scalar(intensity_r, intensity_g, intensity_b));
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

double find_median(vector<double> times)
{
    std::sort(times.begin(), times.end());
    size_t size = times.size();

    if (size % 2 == 0)
        return ((times[size / 2 - 1]) + (times[size / 2])) / 2;
    else
        return (times[size / 2]);
}

double find_average(vector<double> times)
{
    double sum = 0;
    for (int i = 0; i < times.size(); ++i)
    {
        sum += times[i];
    }
    return sum / times.size();
}