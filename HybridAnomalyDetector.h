

#ifndef HYBRIDANOMALYDETECTOR_H_
#define HYBRIDANOMALYDETECTOR_H_

#include "SimpleAnomalyDetector.h"
#include "minCircle.h"

class HybridAnomalyDetector:public SimpleAnomalyDetector {
    //Point** features_points;
public:
	HybridAnomalyDetector();
    virtual ~HybridAnomalyDetector();
    float learnHelper(float pearson_value, bool *correlations_flags,int i,int j, float* i_feature, float* j_feature, int feature_size);
    void learnHelper_set_cf(int feature_vector_size, float *i_feature, float *j_feature,correlatedFeatures *cf_ptr,  int feature_size);
    tuple<float, int> calc_biggest_delta_value_and_ts(float* i_feature, float* j_feature, correlatedFeatures& c, int size);
    void add_Anomalys_2Vector( vector<AnomalyReport>& anomalys, float* i_feature, float* j_feature, correlatedFeatures& c, int size);
    float getThreshold_value();
    void setThreshold_value(float Threshold_value_new);
};

#endif /* HYBRIDANOMALYDETECTOR_H_ */
