

#ifndef SIMPLEANOMALYDETECTOR_H_
#define SIMPLEANOMALYDETECTOR_H_

#include "anomaly_detection_util.h"
#include "AnomalyDetector.h"
#include <vector>
#include <algorithm>
#include <string.h>
#include <math.h>

struct correlatedFeatures{
	string feature1,feature2;  // names of the correlated features
	float corrlation;
	Line lin_reg;
	float threshold;
	float x_center;
    float y_center;
};
//correlatedFeatures new_correlatedFeatures(string feature1,string feature2,float corrlation,Line lin_reg,);

class SimpleAnomalyDetector:public TimeSeriesAnomalyDetector{
protected:
    vector<correlatedFeatures> cf;
    float threshold_value = 0.9;
    Line create_linear_reg_Line_from_2_features(float* i_feature, float* j_feature,int size);
    float calc_biggest_delta(float* i_feature, float* j_feature, Line& line, int size);
public:
	SimpleAnomalyDetector();
	virtual ~SimpleAnomalyDetector();
	virtual void learnNormal(const TimeSeries& ts);
    virtual float learnHelper(float pearson_value, bool *correlations_flags,int i,int j, float* i_feature, float* j_feature, int feature_size);
    virtual void learnHelper_set_cf(int feature_vector_size, float *i_feature, float *j_feature,correlatedFeatures *cf_ptr,  int feature_size);
    virtual void add_Anomalys_2Vector( vector<AnomalyReport>& anomalys, float* i_feature, float* j_feature, correlatedFeatures& c, int size);
    virtual bool isAnomaly(correlatedFeatures &c,tuple<float, int> &value_and_ts);
    virtual vector<AnomalyReport> detect(const TimeSeries& ts);

    virtual tuple<float, int> calc_biggest_delta_value_and_ts(float* i_feature, float* j_feature, correlatedFeatures& c, int size);

	vector<correlatedFeatures> getNormalModel(){
		return cf;
	}
};



#endif /* SIMPLEANOMALYDETECTOR_H_ */
