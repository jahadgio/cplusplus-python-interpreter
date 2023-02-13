#ifndef INTERFACE_H
#define INTERFACE_H

#include <vector>
#include <string>
#include <iostream>



class Interface
{
private:
	typedef std::vector<std::string> programType;
	programType programCode;

public:
	void quit();

	void help(std::string command);
	void help();

	bool read(std::string filename);

	void show();

	void clear();

	void startInterface();
};



#endif