#include "timeseries.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

vector<string>* creatFeatures(const string& s, char delimiter)
{
    vector<string> tokens;
    vector<string>* featuresVector = new vector<string>;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter))
    {
        featuresVector->push_back(token);
    }
    return featuresVector;
}

TimeSeries::TimeSeries(const char *CSVfileName) {
    ifstream input( CSVfileName );
    string line;
    getline( input, line );
    features = creatFeatures(line,',');
    vector<string>* row;
    int size = features->size();
    for(;getline( input, line);)
    {
        num_of_rows++;
        row = creatFeatures(line,',');
        for(int i=0; i<size;i++){
            tsTableMap[(*features)[i]].push_back(stof((*row)[i]));
        }
    }
    input.close();
}

vector<string> * TimeSeries::getFeaturesVector() const{
    return features;
}
vector<float>* TimeSeries::getFeature(string featureName) const{
    return (vector<float>*)&(tsTableMap.at(featureName));
}

int TimeSeries::get_num_of_rows() const{
    return num_of_rows;
}