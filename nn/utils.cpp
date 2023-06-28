#include "utils.h"

double find_median(vector<double> times)
{
    std::sort(times.begin(), times.end());
    size_t size = times.size();

    if (size % 2 == 0)
        return ((times[size / 2 - 1]) + (times[size / 2])) / 2;
    else
        return (times[size / 2]);
}

double find_average(const vector<double>& times)
{
    double sum = 0.0;
    for (int i = 0; i < times.size(); ++i)
    {
        sum += times[i];
    }
    return sum / times.size();
}