

#ifndef CLI_H_
#define CLI_H_

#include <string.h>
#include "commands.h"

using namespace std;

class CLI {
	DefaultIO* dio;
	vector<Command*> CLIs_commands;
	// you can add data members
public:
	CLI(DefaultIO* dio);
	void start();
	virtual ~CLI();

private:
    void add_command(Command* c);
    size_t get_commands_length();
    Command* get_commands_by_index(size_t index);
};

#endif /* CLI_H_ */
