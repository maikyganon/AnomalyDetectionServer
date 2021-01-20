#include "CLI.h"

CLI::CLI(DefaultIO* dio):dio(dio) {
}

void CLI::start(){
    float numeric_input;
    //float * numeric_input_ptr = &numeric_input;
    AnomalysInfo a_Info;
    add_command(new Upload_Ts_Command(dio, a_Info));
    add_command(new Algorithm_Settings_Command(dio, a_Info));
    add_command(new Detect_Anomalies_Command(dio, a_Info));
    add_command(new Display_Results_Command(dio, a_Info));
    add_command(new Upload_Anomalies_And_Analyze_Command(dio, a_Info));
    add_command(new exit_Command(dio, a_Info));
    while (true){
        dio->write("Welcome to the Anomaly Detection Server.\n");
        dio->write("Please choose an option:\n");
        size_t commands_len = get_commands_length();
        for (size_t i= 1;i <= commands_len;i++)
            dio->write(to_string(i) +"."+ get_commands_by_index(i-1)->get_description()+"\n");
        dio->read(&numeric_input);
        if(numeric_input ==6)
            break;
        get_commands_by_index(numeric_input-1)->execute();
    }
}


CLI::~CLI() {
}

void CLI::add_command(Command* C){
    CLIs_commands.push_back(C);
}

size_t CLI::get_commands_length(){
    return CLIs_commands.size();
}

Command* CLI::get_commands_by_index(size_t index){
    return CLIs_commands.at(index);
}
