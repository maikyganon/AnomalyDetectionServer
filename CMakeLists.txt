cmake_minimum_required(VERSION 3.17)
project(MS4)

set(CMAKE_CXX_FLAGS -pthread)

add_executable(MS4 anomaly_detection_util.cpp SimpleAnomalyDetector.cpp timeseries.cpp minCircle.cpp HybridAnomalyDetector.cpp CLI.cpp Server.cpp MainTrain.cpp)