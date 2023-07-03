#include "BaseReader.h"


class CifarReader : public BaseReader
{
public:
    void load_train_data(const string& data_dir, vector<Mat>& images, Mat& labels) override;
    void load_test_data(const string& data_dir, vector<Mat>& images, Mat& labels) override;
private:
    void read_bin_data(int start_index, const string& filename, vector<Mat>& images, Mat& labels);
    string join(const string& p1, const string& p2);
};
