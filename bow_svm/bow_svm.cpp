#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/ml.hpp>

using namespace std;
using namespace cv;
using namespace cv::ml;
using namespace chrono;

#include <opencv2/opencv.hpp>

using namespace cv;


const char* helper =
"./bow_svm <task_type> <data_dir> <svm_file> <vocabulary_file> <output_file>\n\
\t<task_type> - is a task type: <train>, <inference>\n\
\t<data_dir> - is a directory containing data for task cifar10.\n\
\t<svm_file> - is a file to load or save svm model.\n\
\t<vocabulary_file> - is a to load or save vocabulary for BoW.\n\
\t<output_file> - is a file to save the prediction, if task_type = <inference> (.yml format)\n\
";

int trainProccesArgument(int argc, char* argv[], string& data_dir, string& svm_file,
                         string& vocabulary_file);

int inferenceProccesArgument(int argc, char* argv[], string& data_dir, string& svm_file,
                             string& vocabulary_file, string& output_file);

int read_bin_data(int start_index, string filename, vector<Mat>& images, Mat& labels);

int load_data(string data_dir, map<string, int> filenames, vector<Mat>& images, Mat& labels);

void get_keypoints(vector<Mat>& images, vector<vector<KeyPoint>>& keypoints);

void get_keypoints_and_descriptors(vector<Mat>& images, vector<vector<KeyPoint>>& keypoints, 
                                   vector<Mat>& descriptors);

void create_vocabulary(Mat& vocabulary, vector<vector<KeyPoint>>& keypoints,
                       vector<Mat>& descriptors, int vocabulary_size);

void get_features(vector<Mat>& images, vector<vector<KeyPoint>>& keypoints,
                  Mat& vocabulary, Mat& x_data);

void train_svm(Mat train_data, Mat& labels, Ptr<SVM>& svm);

void save_train_artifacts(Ptr<SVM> svm, Mat& vocabulary, string svm_file, string vocabulary_file);

void predict(Ptr<SVM>& svm, Mat train_data, vector<int>& pred);

void train(string data_dir, string svm_file, string vocabulary_file);

void inference(string data_dir, string svm_file, string vocabulary_file, string output_file);

int main(int argc, char* argv[]) 
{
    string task_type, data_dir, svm_file, vocabulary_file, output_file;

    if (string(argv[1]) == "train")
    {
        if (trainProccesArgument(argc, argv, data_dir, svm_file, vocabulary_file) != 0)
        {
            cout << helper << endl;
            return 1;
        }

        train(data_dir, svm_file, vocabulary_file);
    }

    if (string(argv[1]) == "inference")
    {
        if (inferenceProccesArgument(argc, argv, data_dir, svm_file, 
                                     vocabulary_file, output_file) != 0)
        {
            cout << helper << endl;
            return 1;
        }

        inference(data_dir, svm_file, vocabulary_file, output_file);
    }

    return 0;
}


int trainProccesArgument(int argc, char* argv[], string& data_dir, string& svm_file,
    string& vocabulary_file)
{
    if (argc < 5)
    {
        return 1;
    }

    data_dir = argv[2];
    svm_file = argv[3];
    vocabulary_file = argv[4];

    return 0;
}

int inferenceProccesArgument(int argc, char* argv[], string& data_dir, string& svm_file,
                             string& vocabulary_file, string& output_file)
{
    if (argc < 6)
    {
        return 1;
    }

    data_dir = argv[2];
    svm_file = argv[3];
    vocabulary_file = argv[4];
    output_file = argv[5];

    return 0;
}

int read_bin_data(int start_index, string filename, vector<Mat>& images, Mat& labels)
{
    ifstream fs;
    int data_size = 3073;

    char* read_buffer = new char[data_size];

    fs.open(filename, ios::binary);
    if (!fs)
    {
        cerr << "Failed to open file: " << filename << endl;
        return -1;
    }

    for (int i = 0 + start_index; i < labels.rows + start_index; i++) {
        fs.seekg(i * data_size, ios::beg);
        fs.read(read_buffer, data_size);

        Mat im(32, 32, CV_8UC3);

        labels.at<int>(i) = (int)read_buffer[0];
        for (int j = 0; j < 32; j++)
            for (int k = 0; k < 32; k++)
            {
                int b = (int)read_buffer[k + j * 32];
                int g = (int)read_buffer[k + j * 32 + 32 * 32];
                int r = (int)read_buffer[k + j * 32 + 32 * 32 * 2];
                im.at<Vec3b>(j, k) = Vec3b(r, g, b);
            }                
        images.push_back(im.clone());
    }
}

int load_data(string data_dir, map<string, int> filenames, vector<Mat>& images, Mat& labels)
{
    ifstream fs;
    const int tailleImage = 3073;

    map<string, int>::iterator it = filenames.begin();

    while (it != filenames.end())
    {
        if (read_bin_data(it->second, it->first, images, labels) != 0)
            return -1;
    }
}

void get_keypoints(vector<Mat>& images, vector<vector<KeyPoint>>& keypoints)
{
    Ptr<SIFT> sift = SIFT::create();

    for (int i = 0; i < images.size(); i++)
    {
        vector<KeyPoint> keypoint;

        sift->detect(images[i], keypoint, noArray());
        keypoints.push_back(vector<KeyPoint>(keypoint));
    }
}

void get_keypoints_and_descriptors(vector<Mat>& images, vector<vector<KeyPoint>>& keypoints,
                                   vector<Mat>& descriptors)
{
    Ptr<SIFT> sift = SIFT::create();

    for (int i = 0; i < images.size(); i++)
    {
        vector<KeyPoint> keypoint;
        Mat descriptor;

        sift->detectAndCompute(images[i], noArray(), keypoint, descriptor);
        keypoints.push_back(vector<KeyPoint>(keypoint));
        descriptors.push_back(descriptor.clone());
    }
}

void create_vocabulary(Mat& vocabulary, vector<vector<KeyPoint>>& keypoints,
                       vector<Mat>& descriptors, int vocabulary_size)
{
    TermCriteria termCriteria(cv::TermCriteria::EPS + TermCriteria::COUNT, 10, 1.0);
    BOWKMeansTrainer bow(vocabulary_size, termCriteria, 1, KMEANS_PP_CENTERS);

    for (auto& descriptor : descriptors)
    {
        if (descriptor.rows != 0)
            bow.add(descriptor);
    }
    vocabulary = bow.cluster();
}

void get_features(vector<Mat>& images, vector<vector<KeyPoint>>& keypoints,
                  Mat& vocabulary, Mat& x_data)
{
    Ptr<SIFT> sift = SIFT::create();
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("FlannBased");
    BOWImgDescriptorExtractor dextract(sift, matcher);

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

    svm->setC(100);

    svm->setType(SVM::C_SVC);
    svm->setKernel(SVM::RBF);
    svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));

    svm->train(train_data, ROW_SAMPLE, labels);
}

void save_train_artifacts(Ptr<SVM> svm, Mat& vocabulary, string svm_file, string vocabulary_file)
{
    FileStorage fs(vocabulary_file, FileStorage::WRITE);
    fs << "vocabulary" << vocabulary;
    fs.release();

    svm->save(svm_file);
}

void predict(Ptr<SVM>& svm, Mat train_data, vector<int>& pred)
{
    for (int i = 0; i < train_data.rows; i++)
    {
        int predict = (int)svm->predict(train_data.row(i));
        pred.push_back(predict);
    }
}

void train(string data_dir, string svm_file, string vocabulary_file)
{
    vector<Mat> images;
    Mat labels(10000, 1, CV_32SC1);

    map<string, int> filenames = {
        {data_dir + "data_batch_1.bin", 0}, {data_dir + "data_batch_2.bin", 10000},
        {data_dir + "data_batch_3.bin", 20000}, {data_dir + "data_batch_4.bin", 30000},
        {data_dir + "data_batch_5.bin", 40000}
    };

    Mat x_data;

    vector<vector<KeyPoint>> keypoints;
    vector<Mat> descriptors;

    Mat vocabulary;
    Ptr<SVM> svm;

    load_data(data_dir, filenames, images, labels);
    get_keypoints_and_descriptors(images, keypoints, descriptors);
    create_vocabulary(vocabulary, keypoints, descriptors, 1024);

    get_features(images, keypoints, vocabulary, x_data);
    train_svm(x_data, labels, svm);

    save_train_artifacts(svm, vocabulary, svm_file, vocabulary_file);
}

void inference(string data_dir, string svm_file, string vocabulary_file, string output_file)
{
    map<string, int> filenames = { {data_dir + "test_batch.bin", 0} };
    vector<Mat> images;
    Mat labels(10000, 1, CV_32SC1);
    Mat x_data;

    load_data(data_dir, filenames, images, labels);

    Ptr<SVM> svm = Algorithm::load<ml::SVM>(svm_file);

    Mat vocabulary;
    FileStorage fs_read(vocabulary_file, FileStorage::READ);
    fs_read["vocabulary"] >> vocabulary;
    fs_read.release();

    vector<vector<KeyPoint>> keypoints;
    vector<Mat> descriptors;
    get_keypoints(images, keypoints);
    get_features(images, keypoints, vocabulary, x_data);

    vector<int> outputs;
    predict(svm, x_data, outputs);

    FileStorage fs_write(output_file, FileStorage::WRITE);
    fs_write << "outputs" << outputs;
    fs_write.release();
}