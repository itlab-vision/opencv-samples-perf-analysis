#include <iostream>
#include <fstream>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/ml.hpp>

#include "CifarReader.h"

using namespace std;
using namespace cv;
using namespace cv::ml;
using namespace chrono;

const char* helper =
"./bow_svm <task_type> <data_dir> <dataset> <svm_file> <svm_type> <kernel> <max_iter>\
<vocabulary_file> <vocabulary_size> <detector_type> <descriptor_type> <output_file>\n\
\t<task_type> is a task type: <train>, <inference>\n\
\t<data_dir> is a directory containing data.\n\
\t<dataset> is a string containing one of the supported datasets: <cifar10>.\n\
\t<svm_file> is a file to load or save svm model.\n\
\t<svm_type> is a type of a SVM formulation: 100 = C_SVC, 101 = NU_SVC, 102 = ONE_CLASS.\n\
\t<kernel> is a svm kernel: 0 =  LINEAR, 1 = POLY, 2 = RBF, 3 = SIGMOID, 4 = CHI2, 5 = INTER.\n\
\t<max_iter> is a maximum number of iterations to compute svm.\n\
\t<vocabulary_file> is a file to load or save vocabulary for BoW.\n\
\t<vocabulary_size> is a size of vocabulary, only if task_type = <train>\n\
\t<detector_type> is a type detector: <SIFT>, <ORB>, <BRISK>, <MSER>, <AKAZE>, <KAZE>\n\
\t<descriptor_type> is a type descriptor: <SIFT>, <ORB>, <BRISK>, <MSER>, <AKAZE>, <KAZE>\n\
\t<output_file> is a file to save the prediction, if task_type = <inference> (.yml format)\n\
";

int trainProccesArgument(int argc, char* argv[], string& data_dir, 
                         string& dataset, string& svm_file,
                         int& svm_type, int& kernel, int& max_iter,
                         string& vocabulary_file, int& vocabulary_size, 
                         string& detector_type, string& descriptor_type);

int inferenceProccesArgument(int argc, char* argv[], string& data_dir, string& dataset,
                             string& svm_file, string& vocabulary_file, 
                             string& detector_type, string& descriptor_type, 
                             string& output_file);

void get_keypoints(const vector<Mat>& images, vector<vector<KeyPoint>>& keypoints, 
                   Ptr<Feature2D> detector);

void get_keypoints_and_descriptors(const vector<Mat>& images, 
                                   vector<vector<KeyPoint>>& keypoints, 
                                   vector<Mat>& descriptors, Ptr<Feature2D> detector);

void create_vocabulary(Mat& vocabulary, const vector<vector<KeyPoint>>& keypoints,
                       const vector<Mat>& descriptors, int vocabulary_size);

void get_features(const vector<Mat>& images, vector<vector<KeyPoint>>& keypoints,
                  const Mat& vocabulary, Mat& x_data, Ptr<Feature2D> descriptor);

void train_svm(Mat train_data, Mat& labels, Ptr<SVM>& svm, 
               int svm_type, int kernel, int max_count);

void save_train_artifacts(Ptr<SVM> svm, Mat& vocabulary, string svm_file, 
                          string vocabulary_file);

void predict(const Ptr<SVM>& svm, const Mat& train_data, Mat& pred);

void train(const string& data_dir, BaseReader* const& reader, const string& svm_file, 
           int svm_type, int kernel, int max_iter,
           const string& vocabulary_file, int vocabulary_size,
           Ptr<Feature2D> detector, Ptr<Feature2D> descriptor);

void inference(const string& data_dir, BaseReader* const& reader, 
               const string& svm_file, const string& vocabulary_file,
               const string& output_file, Ptr<Feature2D> detector, 
               Ptr<Feature2D> descriptor);

int main(int argc, char* argv[]) 
{
    string task_type, data_dir, dataset, svm_file, vocabulary_file, output_file;
    string detector_type, descriptor_type;
    int vocabulary_size;

    int svm_type, kernel, max_iter;

    const map<string, Ptr<Feature2D>> name_to_2dfeatures = {
        {"SIFT", SIFT::create()}, {"ORB", ORB::create()}, 
        {"BRISK", BRISK::create()}, {"MSER", MSER::create()},
        {"AKAZE", AKAZE::create()}, {"KAZE", KAZE::create()},
    };

    const map<string, BaseReader*> readers = {
        {"cifar10", new CifarReader()}
    };

    if (argc < 2 || !(string(argv[1]) == "train" || string(argv[1]) == "inference"))
    {
        cout << helper << endl;
        return 1;
    }

    if (string(argv[1]) == "train")
    {
        if (trainProccesArgument(argc, argv, data_dir, dataset, svm_file, 
                                 svm_type, kernel, max_iter, vocabulary_file, 
                                 vocabulary_size, detector_type, descriptor_type) != 0)
        {
            cout << helper << endl;
            return 1;
        }
        Ptr<Feature2D> detector = name_to_2dfeatures.at(detector_type);
        Ptr<Feature2D> descriptor = name_to_2dfeatures.at(descriptor_type);

        BaseReader* reader = readers.at(dataset);

        train(data_dir, reader, svm_file, svm_type, kernel, max_iter, 
              vocabulary_file, vocabulary_size, detector, descriptor);
    }

    if (string(argv[1]) == "inference")
    {
        if (inferenceProccesArgument(argc, argv, data_dir, dataset, svm_file, 
                                     vocabulary_file, detector_type, 
                                     descriptor_type, output_file) != 0)
        {
            cout << helper << endl;
            return 1;
        }
        Ptr<Feature2D> detector = name_to_2dfeatures.at(detector_type);
        Ptr<Feature2D> descriptor = name_to_2dfeatures.at(descriptor_type);

        BaseReader* reader = readers.at(dataset);

        inference(data_dir, reader, svm_file, vocabulary_file, 
                  output_file, detector, descriptor);
    }

    return 0;
}


int trainProccesArgument(int argc, char* argv[], string& data_dir, 
                         string& dataset, string& svm_file,
                         int& svm_type, int& kernel, int& max_iter,
                         string& vocabulary_file, int& vocabulary_size, 
                         string& detector_type, string& descriptor_type)
{
    if (argc < 12)
    {
        return 1;
    }

    data_dir = argv[2];
    dataset = argv[3];
    svm_file = argv[4];

    svm_type = atoi(argv[5]);
    kernel = atoi(argv[6]);
    max_iter = atoi(argv[7]);

    vocabulary_file = argv[8];

    vocabulary_size = atoi(argv[9]);

    detector_type = argv[10];
    descriptor_type = argv[11];

    return 0;
}

int inferenceProccesArgument(int argc, char* argv[], string& data_dir, 
                             string& dataset, string& svm_file,
                             string& vocabulary_file, string& detector_type, 
                             string& descriptor_type, string& output_file)
{
    if (argc < 9)
    {
        return 1;
    }

    data_dir = argv[2];
    dataset = argv[3];
    svm_file = argv[4];
    vocabulary_file = argv[5];

    detector_type = argv[6];
    descriptor_type = argv[7];

    output_file = argv[8];

    return 0;
}

void get_keypoints(const vector<Mat>& images, vector<vector<KeyPoint>>& keypoints, 
                   Ptr<Feature2D> detector)
{
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < images.size(); i++)
    {
        vector<KeyPoint> keypoint;

        detector->detect(images[i], keypoint, noArray());
        keypoints.push_back(vector<KeyPoint>(keypoint));
    }
    auto end = std::chrono::steady_clock::now();

    chrono::duration<double> elapsed_seconds = end - start;
    cout << "keypoint detection time: " << elapsed_seconds.count() << "s\n";
}

void get_keypoints_and_descriptors(const vector<Mat>& images, 
                                   vector<vector<KeyPoint>>& keypoints,
                                   vector<Mat>& descriptors, Ptr<Feature2D> detector)
{
    for (int i = 0; i < images.size(); i++)
    {
        vector<KeyPoint> keypoint;
        Mat descriptor;

        detector->detectAndCompute(images[i], noArray(), keypoint, descriptor);
        keypoints.push_back(vector<KeyPoint>(keypoint));
        descriptors.push_back(descriptor.clone());
    }
}

void create_vocabulary(Mat& vocabulary, const vector<vector<KeyPoint>>& keypoints,
                       const vector<Mat>& descriptors, int vocabulary_size)
{
    BOWKMeansTrainer bow(vocabulary_size);

    for (auto& descriptor : descriptors)
    {
        if (descriptor.rows != 0)
            bow.add(descriptor);
    }
    vocabulary = bow.cluster();
}

void get_features(const vector<Mat>& images, vector<vector<KeyPoint>>& keypoints,
                  const Mat& vocabulary, Mat& x_data, Ptr<Feature2D> descriptor)
{
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("FlannBased");
    BOWImgDescriptorExtractor dextract(descriptor, matcher);

    dextract.setVocabulary(vocabulary);

    vector<KeyPoint> keypoint;
    
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < keypoints.size(); i++)
    {
        Mat desc;
        dextract.compute(images[i], keypoints[i], desc);
        if (!desc.empty())
            x_data.push_back(desc);
        else
            x_data.push_back(Mat::zeros(1, x_data.cols, CV_32F));
    }
    auto end = std::chrono::steady_clock::now();

    chrono::duration<double> elapsed_seconds = end - start;
    cout << "feature generation time: " << elapsed_seconds.count() << "s\n";
}

void train_svm(Mat train_data, Mat& labels, Ptr<SVM>& svm, 
               int svm_type, int kernel, int max_count)
{
    svm = SVM::create();

    const double epsilon = 1e-6;

    svm->setType(svm_type);
    svm->setKernel(kernel);
    svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, max_count, 1e-6));

    auto start = std::chrono::steady_clock::now();
    svm->train(train_data, ROW_SAMPLE, labels);
    auto end = std::chrono::steady_clock::now();

    chrono::duration<double> elapsed_seconds = end - start;
    cout << "train time: " << elapsed_seconds.count() << "s\n";
}

void save_train_artifacts(Ptr<SVM> svm, Mat& vocabulary, string svm_file, 
                          string vocabulary_file)
{
    FileStorage fs(vocabulary_file, FileStorage::WRITE);
    fs << "vocabulary" << vocabulary;
    fs.release();

    svm->save(svm_file);
}

void predict(const Ptr<SVM>& svm, const Mat& train_data, Mat& pred)
{
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < train_data.rows; i++)
    {
        int predict = (int)svm->predict(train_data.row(i));
        pred.push_back(predict);
    }
    auto end = std::chrono::steady_clock::now();

    chrono::duration<double> elapsed_seconds = end - start;
    cout << "predict time: " << elapsed_seconds.count() << "s\n";
}

void train(const string& data_dir, BaseReader* const& reader, const string& svm_file, 
           int svm_type, int kernel, int max_iter,
           const string& vocabulary_file, int vocabulary_size,
           Ptr<Feature2D> detector, Ptr<Feature2D> descriptor)
{
    vector<Mat> images;
    Mat labels;

    Mat x_data;

    vector<vector<KeyPoint>> keypoints;
    vector<Mat> descriptors;

    Mat vocabulary;
    Ptr<SVM> svm;
    reader->load_train_data(data_dir, images, labels);
    get_keypoints_and_descriptors(images, keypoints, descriptors, detector);
    create_vocabulary(vocabulary, keypoints, descriptors, vocabulary_size);

    get_features(images, keypoints, vocabulary, x_data, descriptor);
    train_svm(x_data, labels, svm, svm_type, kernel, max_iter);

    save_train_artifacts(svm, vocabulary, svm_file, vocabulary_file);
}

void inference(const string& data_dir, BaseReader* const& reader, 
               const string& svm_file, const string& vocabulary_file,
               const string& output_file, Ptr<Feature2D> detector, 
               Ptr<Feature2D> descriptor)
{
    vector<Mat> images;
    Mat labels;
    Mat x_data;

    reader->load_test_data(data_dir, images, labels);

    Ptr<SVM> svm = Algorithm::load<ml::SVM>(svm_file);

    Mat vocabulary;
    FileStorage fs_read(vocabulary_file, FileStorage::READ);
    fs_read["vocabulary"] >> vocabulary;
    fs_read.release();

    vector<vector<KeyPoint>> keypoints;
    vector<Mat> descriptors;
    get_keypoints(images, keypoints, detector);
    get_features(images, keypoints, vocabulary, x_data, descriptor);

    Mat outputs;
    predict(svm, x_data, outputs);

    FileStorage fs_write(output_file, FileStorage::WRITE);
    fs_write << "outputs" << outputs;
    fs_write << "labels" << labels;
    fs_write.release();
}
