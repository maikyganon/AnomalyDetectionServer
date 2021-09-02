# AnomalyDetectionServer

## Abstract: 

This anomaly detection server is build to find anomalys in flight data using pre-recorded data. The server can handle multiple users simultaneously. Once a user conected to the server he Gets access to a CLI -
![image](https://user-images.githubusercontent.com/16485984/131828883-00c126e3-02e3-4f32-b769-dd5db0099d12.png)

## Files:
- **anomaly_detection_util** - Utilitie for detecting anomalies.
- **TimeSeries** - CSV data management.
- **AnomalyDetector.h** - Anomaly detector Interface.
- **SimpleAnomalyDetector** - 
    -Ability to learn which features are correlative to each other.
    -Building a regrasion line for any correlative features.
    -Find anomalys in new data. 
- **minCircle** - Efficientely finding the circle(center and radius) with the minimum radius that contains all the data points (considered normal).
- **HybridAnomalyDetector** - Combining the simple detector with the find min Circle algorithm. if the correlation threshold is greater than or equal to the set threshold (e.g. 9.0) then the algorithm will run the simple detector, on the other hand, if there are feature whose maximum correlation with another property is greater than 5.0 but
Is smaller than the correlation threshold so for these two features we will use the min circle algorithm (we will find the min circle and a point will be a anomly if it is outside the circle).
![CLI UML](https://user-images.githubusercontent.com/16485984/131841573-1511cf70-b790-4294-a656-72429b2214e5.png)
- **CLI** - Command Line Interface
- **Command.h** - Commands interface
- **MainTrain** - test's
 --
 Kind regards,
 Michael Ganon
