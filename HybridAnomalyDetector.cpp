
#include "HybridAnomalyDetector.h"

HybridAnomalyDetector::HybridAnomalyDetector() {}

float HybridAnomalyDetector::learnHelper(float pearson_value, bool *correlations_flags,int i,int j, float* i_feature, float* j_feature, int feature_size){
    if(pearson_value >= threshold_value){
        correlations_flags[i] = true;
        return pearson_value ;
    } else if(pearson_value > 0.5 ){
        correlations_flags[i] = true;
        return pearson_value ;
    }
    else
        return -1.0;
}

void HybridAnomalyDetector::learnHelper_set_cf(int feature_vector_size, float *i_feature, float *j_feature,
                                               correlatedFeatures *cf_ptr,  int feature_size){
    if(cf_ptr->corrlation >= threshold_value){
        cf_ptr->lin_reg = SimpleAnomalyDetector::create_linear_reg_Line_from_2_features(i_feature, j_feature, feature_vector_size);
        cf_ptr->threshold = SimpleAnomalyDetector::calc_biggest_delta(i_feature, j_feature, cf_ptr->lin_reg, feature_vector_size) * 1.1;
        cf_ptr->x_center = 0;
        cf_ptr->y_center = 0;
    } else if(cf_ptr->corrlation > 0.5){
        cf_ptr->lin_reg = SimpleAnomalyDetector::create_linear_reg_Line_from_2_features(i_feature, j_feature, feature_vector_size);
        Point* features_points[feature_size];
        for (int i = 0; i < feature_size; ++i) {
            features_points[i] = new Point(i_feature[i], j_feature[i]);
        }
        Circle minCircle = findMinCircle(features_points, feature_size);
        cf_ptr->threshold =  minCircle.radius * 1.1;
        cf_ptr->x_center = minCircle.center.x;
        cf_ptr->y_center = minCircle.center.y;
    }
}

void HybridAnomalyDetector::add_Anomalys_2Vector( vector<AnomalyReport>& anomalys, float* i_feature, float* j_feature,
                                                  correlatedFeatures& c, int size){
    if(c.corrlation >= threshold_value){
        SimpleAnomalyDetector::add_Anomalys_2Vector(anomalys, i_feature,j_feature,
                             c, size);
    }else if(c.corrlation > 0.5){
        float distance = 0;
        float radios = c.threshold;
        Point p_k(0.0,0.0);
        Point center_c(c.x_center, c.y_center);
        for (int k = 0; k < size; ++k) {
            p_k.x =i_feature[k];
            p_k.y =j_feature[k];
            distance = distanceBetweenTwoPoints(p_k, center_c);
            //distance = abs(j_feature[k] - (c.lin_reg).f(i_feature[k]));
            if(distance > radios) {
                anomalys.push_back(*(new AnomalyReport((c.feature1 + "-" + c.feature2),
                                                       k+1)));
            }
        }
    }
}

tuple<float, int> HybridAnomalyDetector::calc_biggest_delta_value_and_ts(float* i_feature, float* j_feature, correlatedFeatures& c, int size){
    if(c.corrlation >= threshold_value){
        return SimpleAnomalyDetector::calc_biggest_delta_value_and_ts(i_feature, j_feature, c, size);
    } else {
        float maxTs = 0;
        int maxTsRow = 0;
        float distance = 0;
        Point p_k(0.0,0.0);
        Point center_c(c.x_center, c.y_center);
        for (int k = 0; k < size; ++k) {
            p_k.x =i_feature[k];
            p_k.y =j_feature[k];
            distance = distanceBetweenTwoPoints(p_k, center_c);
            //distance = abs(j_feature[k] - (c.lin_reg).f(i_feature[k]));
            if(distance > maxTs) {
                maxTs = distance;
                maxTsRow = k + 1;
            }
        }
        return make_tuple(maxTs, maxTsRow);
    }
}

float HybridAnomalyDetector::getThreshold_value(){
    return threshold_value;
}
void HybridAnomalyDetector::setThreshold_value(float Threshold_value_new){
    threshold_value = Threshold_value_new;
}

HybridAnomalyDetector::~HybridAnomalyDetector() {
	// TODO Auto-generated destructor stub
}

