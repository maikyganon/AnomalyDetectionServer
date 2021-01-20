
#ifndef TIMESERIES_H_
#define TIMESERIES_H_
#include <vector>
#include <map>
using namespace std;

class TimeSeries{
    map<string,vector<float>> tsTableMap;
    vector<string>* features;
    int num_of_rows = 0;
public:
	TimeSeries(const char* CSVfileName);
    vector<string>* getFeaturesVector() const;
    vector<float>* getFeature(string featureName) const;
    int get_num_of_rows() const;
};



#endif /* TIMESERIES_H_ */
