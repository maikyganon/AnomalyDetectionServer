

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include<iostream>
#include <string.h>

#include <fstream>
#include <vector>
#include "HybridAnomalyDetector.h"

using namespace std;

class DefaultIO{
public:
	virtual string read()=0;
	virtual void write(string text)=0;
	virtual void write(float f)=0;
	virtual void read(float* f)=0;
	virtual ~DefaultIO(){}

	// you may add additional methods here
	int download_CSVs(string dstFile) {
	    int countRows_Until_done = 0;
        ofstream os;
        os.open(dstFile);
        string s = read();
        while(s.compare("done")){
            countRows_Until_done++;
            os << s + "\n";
            s = read();
        }
        os.close();
        write("Upload complete.\n");
        return countRows_Until_done;
    }

    vector<tuple<size_t, size_t>> download_CSVs() {
        vector<tuple<size_t, size_t>> rows;
        string row = read();
        size_t comma_pos, first, second;
        while(row.compare("done")){
            comma_pos = row.find_first_of(',');
            first = stoi(row.substr(0,comma_pos));
            second = stoi(row.substr(comma_pos + 1));
            rows.push_back(make_tuple(first, second));
            row = read();
        }
        write("Upload complete.\n");
       return rows;
    }

};



// you may add here helper classes
struct AnomalysInfo{
    HybridAnomalyDetector had;
    vector<AnomalyReport> anomalys_vec;
    size_t test_length;
};


// you may edit this class
class Command{
protected:
    //const string description;
    string description;
    AnomalysInfo& anomalysInfo;
	DefaultIO* dio;
public:
	Command(DefaultIO* dio, string desc, AnomalysInfo& aI):dio(dio), description(desc), anomalysInfo(aI){}
    virtual string get_description() {
        return description;
    }
	virtual void execute()=0;
	virtual ~Command(){}
};

// implement here your command classes
class Upload_Ts_Command : public Command{

public:
    Upload_Ts_Command(DefaultIO* dio, AnomalysInfo& aI):Command(dio, "upload a time series csv file", aI){}
    virtual void execute() {
        dio->write("Please upload your local train CSV file.\n");
        dio->download_CSVs("anomalyTrain.csv");
        dio->write("Please upload your local test CSV file.\n");
        anomalysInfo.test_length = dio->download_CSVs("anomalyTest.csv") - 1;
    }
};

class Algorithm_Settings_Command : public Command{
    //const string Algorithm_Settings_Command_description = " algorithm settings";
public:
    Algorithm_Settings_Command(DefaultIO* dio, AnomalysInfo& aI):Command(dio, "algorithm settings", aI){}
    virtual void execute() {
        float numeric_input;
        dio->write("The current correlation threshold is " );
        dio->write(anomalysInfo.had.getThreshold_value());
        dio->write("\n");
        while(true){
            dio->write("Type a new threshold\n");
            dio->read(&numeric_input);
            if(numeric_input>=0 && numeric_input<=1){
                anomalysInfo.had.setThreshold_value(numeric_input);
                break;
            } else {
                dio->write("please choose a value between 0 and 1.\n");
            }
        }
    }
};

class Detect_Anomalies_Command : public Command{
    //const string Algorithm_Settings_Command_description = " algorithm settings";
public:
    Detect_Anomalies_Command(DefaultIO* dio, AnomalysInfo& aI):Command(dio, "detect anomalies", aI){}
    virtual void execute() {
        TimeSeries ts_train("anomalyTrain.csv");
        anomalysInfo.had.learnNormal(ts_train);
        TimeSeries ts_test("anomalyTest.csv");
        anomalysInfo.anomalys_vec = anomalysInfo.had.detect(ts_test);
        dio->write("anomaly detection complete.\n");
    }
};

class Display_Results_Command : public Command{
    //const string Algorithm_Settings_Command_description = " algorithm settings";
public:
    Display_Results_Command(DefaultIO* dio, AnomalysInfo& aI):Command(dio, "display results", aI){}
    virtual void execute() {
        auto it_end = end (anomalysInfo.anomalys_vec);
        for (auto it = begin (anomalysInfo.anomalys_vec); it != it_end; ++it) {
            dio->write(to_string(it->timeStep) + "\t" + it->description + "\n");
        }
        dio->write("Done.\n");
    }
};

class Upload_Anomalies_And_Analyze_Command : public Command{
    //const string Algorithm_Settings_Command_description = " algorithm settings";
public:
    Upload_Anomalies_And_Analyze_Command(DefaultIO* dio, AnomalysInfo& aI):Command(dio, "upload anomalies and analyze results", aI){}

    bool is_overlap( size_t client_start,size_t detected_start,size_t client_stop,size_t detected_stop){
        if((client_start == detected_start) && (client_stop == detected_stop))
            return true;
        if(((client_start >= detected_start) && (client_start <= detected_stop))||((client_stop >= detected_start) && (client_stop <= detected_stop)))
            return true;
        if(((detected_start >= client_start) && (detected_start <= client_stop))||((detected_stop >= client_start) && (detected_stop <= client_stop)))
            return true;
        return false;
    }

    virtual void execute() {
        int TP = 0,P,FP = 0,N;
        vector<tuple<size_t, size_t>> detected_errors;
        size_t first = 0,second = 0;
        string last_description;
        if(anomalysInfo.anomalys_vec.size() > 0){
            last_description = anomalysInfo.anomalys_vec.at(0).description;
            first = anomalysInfo.anomalys_vec.at(0).timeStep;
            second = first;
            detected_errors.push_back(make_tuple(first, second));
            auto anomalys_it_end = end (anomalysInfo.anomalys_vec);
            auto itr = begin (anomalysInfo.anomalys_vec) +1;
            for (; itr != anomalys_it_end; ++itr) {
                if(last_description.compare(itr->description) == 0){
                       if(second + 1 == itr->timeStep){
                           second++;
                           get<1>(detected_errors.back())++;
                       }else{
                           first =itr->timeStep;
                           second = first;
                           detected_errors.push_back(make_tuple(first, second));
                       }
                }else{
                    first =itr->timeStep;
                    second = first;
                    detected_errors.push_back(make_tuple(first, second));
                    last_description = itr->description;
                }
            }
        }

        dio->write("Please upload your local anomalies file.\n");

        vector<tuple<size_t , size_t>> client_errors= dio->download_CSVs();
        P = client_errors.size();
        N = anomalysInfo.test_length;
        bool TP_flag = false;
        vector<bool> TP_or_FP (detected_errors.size(), false);
        size_t client_start, detected_start, client_stop, detected_stop;
        auto client_errors_it_end = end (client_errors);
        for (auto client_errors_it = begin (client_errors); client_errors_it != client_errors_it_end; ++client_errors_it) {
            TP_flag = false;
            client_start = get<0>(*client_errors_it);
            client_stop = get<1>(*client_errors_it);
            N = N - (client_stop - client_start + 1);

            if(anomalysInfo.anomalys_vec.size() > 0){
                auto detected_it_end = end (detected_errors);
                auto detected_itr = begin (detected_errors);
                for (; detected_itr != detected_it_end; ++detected_itr) {
                    detected_start = get<0>(*detected_itr);
                    detected_stop = get<1>(*detected_itr);
                    if(is_overlap(  client_start, detected_start, client_stop, detected_stop)){
                        if(TP_flag == false)
                            TP++;
                        TP_flag =true;
                        TP_or_FP.at(detected_itr - begin (detected_errors)) = true;
                    }
                }
            }

        }
        auto flags_end = end (TP_or_FP);
        auto flags_itr = begin (TP_or_FP);
        for (; flags_itr != flags_end; ++flags_itr) {
            if(*flags_itr == false)
                FP++;
        }
        float TPR =  0, FAR = 0;
        if(P != 0)
            TPR = float(TP) / P;
        if(N != 0)
            FAR = float(FP) / N;
        dio->write("True Positive Rate: ");
        dio->write(floor(TPR*1000) / 1000);
        dio->write("\n");
        dio->write("False Positive Rate: ");
        dio->write(floor(FAR*1000) / 1000);
        dio->write("\n");
    }
};

class exit_Command : public Command{
    //const string Algorithm_Settings_Command_description = " algorithm settings";
public:
    exit_Command(DefaultIO* dio, AnomalysInfo& aI):Command(dio, "exit", aI){}
    virtual void execute() {

    }
};

#endif /* COMMANDS_H_ */
