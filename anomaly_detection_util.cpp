#include "anomaly_detection_util.h"
#include <stdlib.h>
#include <math.h>

//calc AVG
float E(float* x, int size){
    float sum = 0;
    float sumAvg;
    for (int i = 0; i < size; i++)
    {
        sum += x[i];
    }
    sumAvg = sum / size;
    return sumAvg;
}
// returns the variance of X and Y
float var(float* x, int size){
    float sum = 0, sumAvg, sumSquared = 0,sumSquaredAvg;
    for (int i = 0; i < size; i++)
    {
        sumSquared += x[i]*x[i];
        sum += x[i];
    }
    sumSquaredAvg = sumSquared / size;
    sumAvg = sum / size;
    return sumSquaredAvg - (sumAvg * sumAvg);
}
// returns the covariance of X and Y
 float cov(float* x, float* y, int size){
    float xy[size];
    for (int i = 0; i < size; i++)
    {
        xy[i] = x[i]*y[i];
    }
    return E(xy,size) - (E(x,size)*E(y,size));
 }
// returns the Pearson correlation coefficient of X and Y
 float pearson(float* x, float* y, int size){
     return cov(x,y,size)/((sqrt(var(x,size)))*(sqrt(var(y,size))));
 }
// performs a linear regression and return s the line equation
Line linear_reg(Point** points, int size){
    float x[size], y[size];
    for (int i = 0; i < size; i++)
    {
        x[i] = points[i]->x;
        y[i] = points[i]->y;
    }
    float a = cov(x,y,size)/var(x,size);
    float b = E(y,size) - (a * E(x,size));
    Line l(a,b);
    return l;
}
// returns the deviation between point p and the line equation of the points
float dev(Point p,Point** points, int size){
    Line l = linear_reg(points, size);
    return fabs(l.f(p.x) - p.y);
}
// returns the deviation between point p and the line
float dev(Point p,Line l){
    return fabs(l.f(p.x) - p.y);
}