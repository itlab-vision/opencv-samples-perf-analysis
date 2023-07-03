#include "CifarReader.h"

void CifarReader::load_train_data(const string& data_dir, vector<Mat>& images, Mat& labels)
{
    labels.create(50000, 1, CV_32SC1);

    map<string, int> filenames = {
        {join(data_dir, "data_batch_1.bin"), 0}, {join(data_dir, "data_batch_2.bin"), 10000},
        {join(data_dir, "data_batch_3.bin"), 20000}, {join(data_dir, "data_batch_4.bin"), 30000},
        {join(data_dir, "data_batch_5.bin"), 40000}
    };
    ifstream fs;
    const int tailleImage = 3073;

    map<string, int>::iterator it = filenames.begin();

    while (it != filenames.end())
    {
        read_bin_data(it->second, it->first, images, labels);
        it++;
    }
}

void CifarReader::load_test_data(const string& data_dir, vector<Mat>& images, Mat& labels)
{
    labels.create(10000, 1, CV_32SC1);

    map<string, int> filenames = { {join(data_dir, "test_batch.bin"), 0} };

    ifstream fs;
    const int tailleImage = 3073;

    map<string, int>::iterator it = filenames.begin();

    read_bin_data(it->second, it->first, images, labels);
}

void CifarReader::read_bin_data(int start_index, const string& filename, 
                                vector<Mat>& images, Mat& labels)
{
    ifstream fs;
    int data_size = 3073;

    char* read_buffer = new char[data_size];

    fs.open(filename, ios::binary);
    if (!fs)
    {
        cerr << "Failed to open file: " << filename << endl;
    }

    int size = images.size();

    for (int i = 0 + start_index; i < 10000 + start_index; i++) {
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

string CifarReader::join(const string& p1, const string& p2)
{
    char sep = '/';
    string tmp = p1;

#ifdef _WIN32
    sep = '\\';
#endif

    // Add separator if it is not included in the first path:
    if (p1[p1.length() - 1] != sep) {
        tmp += sep;
        return tmp + p2;
    } else {
        return p1 + p2;
    }
}
