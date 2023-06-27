#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/ml.hpp>

#include "CifarReader.h"

using namespace std;
using namespace cv;
using namespace cv::ml;
using namespace chrono;

const char* helper =
"./bow_svm <task_type> <data_dir> <dataset> <svm_file> <vocabulary_file>\
<vocabulary_size> <detector_type> <descriptor_type> <output_file>\n\
\t<task_type> is a task type: <train>, <inference>\n\
\t<data_dir> is a directory containing data.\n\
\t<dataset> is a string containing one of the supported datasets: <cifar10>.\n\
\t<svm_file> is a file to load or save svm model.\n\
\t<vocabulary_file> is a file to load or save vocabulary for BoW.\n\
\t<vocabulary_size> is a size of vocabulary, only if task_type = <train>\n\
\t<detector_type> is a type detector.\n\
\t<descriptor_type> is a type descriptor.\n\
\t<output_file> is a file to save the prediction, if task_type = <inference> (.yml format)\n\
";

int trainProccesArgument(int argc, char* argv[], string& data_dir, string& dataset, 
                         string& svm_file, string& vocabulary_file, int& vocabulary_size, 
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

void train_svm(Mat train_data, Mat& labels, Ptr<SVM>& svm);

void save_train_artifacts(Ptr<SVM> svm, Mat& vocabulary, string svm_file, 
                          string vocabulary_file);

void predict(const Ptr<SVM>& svm, const Mat& train_data, vector<int>& pred);

void train(const string& data_dir, BaseReader* const& reader, const string& svm_file, 
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

    const map<string, Ptr<Feature2D>> m = {
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
        if (trainProccesArgument(argc, argv, data_dir, dataset, svm_file, vocabulary_file, 
                vocabulary_size, detector_type, descriptor_type) != 0)
        {
            cout << helper << endl;
            return 1;
        }
        Ptr<Feature2D> detector = m.at(detector_type);
        Ptr<Feature2D> descriptor = m.at(descriptor_type);

        BaseReader* reader = readers.at(dataset);

        train(data_dir, reader, svm_file, vocabulary_file, 
            vocabulary_size, detector, descriptor);
    }

    if (string(argv[1]) == "inference")
    {
        if (inferenceProccesArgument(argc, argv, data_dir, dataset, svm_file, 
                vocabulary_file, detector_type, descriptor_type, output_file) != 0)
        {
            cout << helper << endl;
            return 1;
        }
        Ptr<Feature2D> detector = m.at(detector_type);
        Ptr<Feature2D> descriptor = m.at(descriptor_type);

        BaseReader* reader = readers.at(dataset);

        inference(data_dir, reader, svm_file, vocabulary_file, 
            output_file, detector, descriptor);
    }

    return 0;
}


int trainProccesArgument(int argc, char* argv[], string& data_dir, 
                         string& dataset, string& svm_file,
                         string& vocabulary_file, int& vocabulary_size, 
                         string& detector_type, string& descriptor_type)
{
    if (argc < 9)
    {
        return 1;
    }

    data_dir = argv[2];
    dataset = argv[3];
    svm_file = argv[4];
    vocabulary_file = argv[5];

    vocabulary_size = atoi(argv[6]);

    detector_type = argv[7];
    descriptor_type = argv[8];

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
    for (int i = 0; i < images.size(); i++)
    {
        vector<KeyPoint> keypoint;

        detector->detect(images[i], keypoint, noArray());
        keypoints.push_back(vector<KeyPoint>(keypoint));
    }
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
    for (int i = 0; i < keypoints.size(); i++)
    {
        Mat desc;
        dextract.compute(images[i], keypoints[i], desc);
        if (!desc.empty())
            x_data.push_back(desc);
        else
            x_data.push_back(Mat::zeros(1, x_data.cols, CV_32F));
    }
}

void train_svm(Mat train_data, Mat& labels, Ptr<SVM>& svm)
{
    svm = SVM::create();

    svm->setKernel(SVM::RBF);
    svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));

    svm->train(train_data, ROW_SAMPLE, labels);
}

void save_train_artifacts(Ptr<SVM> svm, Mat& vocabulary, string svm_file, 
                          string vocabulary_file)
{
    FileStorage fs(vocabulary_file, FileStorage::WRITE);
    fs << "vocabulary" << vocabulary;
    fs.release();

    svm->save(svm_file);
}

void predict(const Ptr<SVM>& svm, const Mat& train_data, vector<int>& pred)
{
    for (int i = 0; i < train_data.rows; i++)
    {
        int predict = (int)svm->predict(train_data.row(i));
        pred.push_back(predict);
    }
}

void train(const string& data_dir, BaseReader* const& reader, const string& svm_file, 
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
    train_svm(x_data, labels, svm);

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

    vector<int> outputs;
    predict(svm, x_data, outputs);

    FileStorage fs_write(output_file, FileStorage::WRITE);
    fs_write << "outputs" << outputs;
    fs_write.release();
}