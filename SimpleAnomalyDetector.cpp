
#include "SimpleAnomalyDetector.h"

float SimpleAnomalyDetector::calc_biggest_delta(float* i_feature, float* j_feature, Line& line, int size){
    float maxTs = 0;
    float distance = 0;
    for (int k = 0; k < size; ++k) {
        distance = abs(j_feature[k] - line.f(i_feature[k]));
        if(distance > maxTs)
            maxTs = distance;
    }
    return maxTs;
}
tuple<float, int> SimpleAnomalyDetector::calc_biggest_delta_value_and_ts(float* i_feature, float* j_feature, correlatedFeatures& c, int size){
    float maxTs = 0;
    int maxTsRow = 0;
    float distance = 0;
    for (int k = 0; k < size; ++k) {
        distance = abs(j_feature[k] - (c.lin_reg).f(i_feature[k]));
        if(distance > maxTs)
        {
            maxTs = distance;
            maxTsRow = k + 1;
        }
    }
    return make_tuple(maxTs, maxTsRow);
}

Line SimpleAnomalyDetector::create_linear_reg_Line_from_2_features(float* i_feature, float* j_feature,int size){
    Point* features_points[size];
    for (int i = 0; i < size; ++i) {
        features_points[i] = new Point(i_feature[i], j_feature[i]);
    }
    return linear_reg(features_points,size);
}

SimpleAnomalyDetector::SimpleAnomalyDetector() {
	// TODO Auto-generated constructor stub

}

SimpleAnomalyDetector::~SimpleAnomalyDetector() {
	// TODO Auto-generated destructor stub
}


void SimpleAnomalyDetector::learnNormal(const TimeSeries& ts){
    const vector<string>* features_vec = ts.getFeaturesVector();
    size_t features_len= features_vec->size();
    int feature_vector_size = ts.get_num_of_rows();
    float* i_feature;
    float* j_feature;
    float pearson_value;
    bool correlations_flags[features_len];
    float pearsons[features_len][features_len];
    for (int i = 0; i < features_len; i++) {
        correlations_flags[i] = false;
        i_feature = (*(ts.getFeature((*features_vec)[i]))).data();
        for (int j = i + 1; j < features_len; j++) {
            j_feature = (*(ts.getFeature((*features_vec)[j]))).data();
            pearson_value = pearson(i_feature, j_feature, feature_vector_size);
            pearson_value = fabs(pearson_value);
            //learnHelper(pearson_value);
            pearsons[i][j] = learnHelper(pearson_value, correlations_flags, i, j, i_feature, j_feature, feature_vector_size);
        }
    }
    float max_pearson;
    int max_pearsons_j;
    correlatedFeatures* cf_ptr;
    for (int i = 0; i < features_len; i++) {
        if(correlations_flags[i]){
            max_pearson = 0;
            for (int j = i + 1; j < features_len; j++) {
                if(pearsons[i][j]>max_pearson){
                    max_pearsons_j=j;
                    max_pearson = pearsons[i][j];
                }
            }
            correlatedFeatures cf_new;
            cf.push_back(cf_new);
            cf_ptr = &cf.back();
            cf_ptr->feature1 = (*features_vec)[i];
            cf_ptr->feature2 = (*features_vec)[max_pearsons_j];
            cf_ptr->corrlation = max_pearson;
            i_feature = (*(ts.getFeature((*features_vec)[i]))).data();
            j_feature = (*(ts.getFeature((*features_vec)[max_pearsons_j]))).data();
            learnHelper_set_cf(feature_vector_size, i_feature, j_feature, cf_ptr, feature_vector_size);
        }
    }
}

void SimpleAnomalyDetector::learnHelper_set_cf(int feature_vector_size, float *i_feature, float *j_feature,
                                               correlatedFeatures *cf_ptr,  int feature_size){
    cf_ptr->lin_reg = create_linear_reg_Line_from_2_features(i_feature, j_feature, feature_vector_size);
    cf_ptr->threshold = calc_biggest_delta(i_feature, j_feature, cf_ptr->lin_reg, feature_vector_size) * 1.1;
}

float SimpleAnomalyDetector::learnHelper(float pearson_value, bool *correlations_flags,int i,int j, float* i_feature, float* j_feature, int feature_size){
    if(pearson_value >= threshold_value){
        correlations_flags[i] = true;
        return pearson_value;
    }
    else
        return -1.0;
}

vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries& ts){
    const vector<string>* features_vec = ts.getFeaturesVector();
    size_t features_len= features_vec->size();
    int feature_vector_size = ts.get_num_of_rows();
    vector<AnomalyReport> anomalys;
    for_each(cf.begin(),cf.end(),[this, &feature_vector_size, &ts, &anomalys](correlatedFeatures c){
        add_Anomalys_2Vector(anomalys, ts.getFeature(c.feature1)->data(),ts.getFeature(c.feature2)->data(),
                             c, feature_vector_size);

//
//        auto value_and_ts = calc_biggest_delta_value_and_ts(ts.getFeature(c.feature1)->data(),
//                                                            ts.getFeature(c.feature2)->data(),
//                                                            c, feature_vector_size);
//        if(get<0>(value_and_ts) > c.threshold){
//            anomalys.push_back(*(new AnomalyReport((c.feature1 + "-" + c.feature2),
//                                                   get<1>(value_and_ts))));
//        }
    });
    return anomalys;
}

void SimpleAnomalyDetector::add_Anomalys_2Vector( vector<AnomalyReport>& anomalys, float* i_feature, float* j_feature,
                                                  correlatedFeatures& c, int size){
    float distance = 0;
    for (int k = 0; k < size; ++k) {
        distance = abs(j_feature[k] - (c.lin_reg).f(i_feature[k]));
        if(distance > c.threshold){
            anomalys.push_back(*(new AnomalyReport((c.feature1 + "-" + c.feature2),
                                                  k+1)));
        }
    }
}

bool SimpleAnomalyDetector::isAnomaly(correlatedFeatures &c,tuple<float, int> &value_and_ts) {
    return get<0>(value_and_ts) > c.threshold;
}

