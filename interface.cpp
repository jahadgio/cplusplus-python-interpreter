#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "interface.h"
#include "lexanalyzer.h"
#include "expevaluator.h"
#include "interpreter.h"

using namespace std;

void Interface::startInterface()
{
	string userInput;
	LexicalAnalyzer lexanalysis;
	expEvaluator expEvaluation;
	Interpreter pysubi;

getout:
	while (1)
	{
		cout << ">>> "; // for python terminal appearance
		getline(cin, userInput); // get input

		// take user input and make it lower case
		for (int i = 0; i < userInput.length(); i++)
			userInput[i] = tolower(userInput[i]);

		// if statement skyscraper to find out what command they inputted
		if (userInput == "quit" || userInput == "quit()")
			Interface::quit();

		// help interface
		else if (userInput == "help" || userInput == "help()")
			Interface::help();
		else if (userInput.substr(0, 5) == "help(") // checks so that we can grab the parameter put into the command
		{
			if (userInput.back() == ')') // makes sure there's a closing parentheses at the end
			{
				string inp;
				inp = userInput.substr(5, userInput.length() - 6);
				help(inp);
			}
		}

		// to read a file
		else if (userInput == "read")
			cout << "Error: no parameters given." << endl;
		else if (userInput.substr(0, 5) == "read(") // checks so that we can grab the parameter put into the command
		{
			if (userInput.back() == ')') // makes sure there's a closing parentheses at the end
			{
				string inp;
				inp = userInput.substr(5, userInput.length() - 6); // grabs the string inside the parentheses
				if (read(inp)) // if read worked then run lexanalysis and clear programCode and lexanalysis
				{
					lexanalysis.clear();
					lexanalysis.createPairs(programCode);
				}
			}
			else
				cout << userInput << " is not recognized as a command." << endl << endl;
		}
		// show the loaded file
		else if (userInput == "show" || userInput == "show()")
			Interface::show();

		// show tokens from lexanalyzer
		else if (userInput == "show(tokens)")
			lexanalysis.showTokens();

		else if (userInput == "show(variables)")
			expEvaluation.showSymT();

		// clear the loaded file
		else if (userInput == "clear" || userInput == "clear()")
		{
			lexanalysis.clear();
			clear();
			expEvaluation.clearSymT();
		}

		else if (userInput == "run" || userInput == "run()")
		{
			if (lexanalysis.getTokens().empty())
			{
				cout << "There is no code to run. Please read a file using the read() command and try again." << endl;
			}

			// running all the lines through using a for loop
			for (int linenum = 0; linenum < lexanalysis.getTokens().size(); linenum++)
			{
				if (!pysubi.evalLine(linenum, lexanalysis.getTokens(), expEvaluation)) // if evalLine returns false then it will not continue running
					break;
			}
		}
		else if (userInput.substr(0, 5) == "while")
		{
			if (userInput[userInput.size() - 1] != ':')
			{
				cout << "ERROR: invalid syntax" << endl;
			}
			else
			{
				vector<string> tempcode;
				tempcode.push_back(userInput); // push while statement into tempcode
				cout << "...";
				while (getline(cin, userInput) && userInput != "")
				{
					if (userInput[0] != '\t')
					{
						cout << "ERROR: invalid syntax" << endl;
						goto getout;
					}
					tempcode.push_back(userInput);
					cout << "...";
				}

				lexanalysis.createPairs(tempcode);
				// running all the lines through using a for loop
				for (int linenum = 0; linenum < lexanalysis.getTokens().size(); linenum++)
				{
					if (!pysubi.evalLine(linenum, lexanalysis.getTokens(), expEvaluation)) // if evalLine returns false then it will not continue running
						break;
				}
			}
		}

		else if (userInput.substr(0, 2) == "if")
		{
			if (userInput[userInput.size() - 1] != ':')
			{
				cout << "ERROR: invalid syntax" << endl;
			}
			else
			{
				vector<string> tempcode;
				tempcode.push_back(userInput); // push while statement into tempcode
				cout << "...";
				while (getline(cin, userInput) && userInput != "")
				{
					if (userInput[0] != '\t')
					{
						cout << "ERROR: invalid syntax" << endl;
						goto getout;
					}
					tempcode.push_back(userInput);
					cout << "...";
				}

				lexanalysis.createPairs(tempcode);
				// running all the lines through using a for loop
				for (int linenum = 0; linenum < lexanalysis.getTokens().size(); linenum++)
				{
					if (!pysubi.evalLine(linenum, lexanalysis.getTokens(), expEvaluation)) // if evalLine returns false then it will not continue running
						break;
				}
			}
		}
		else if (userInput.find('=') != string::npos)
			{
				vector<string> tempcode = { userInput };
				lexanalysis.createPairs(tempcode);
				if (expEvaluation.toPostfix(lexanalysis.getTokens()[0]).empty())
				{
					cout << userInput << " is not recognized as a command" << endl << endl;
				}
				else
				{
					int linenum = 0;
					pysubi.evalLine(linenum, lexanalysis.getTokens(), expEvaluation);
				}
		}
		// for direct calculation of input of expression, uses expEval
		else if (isalpha(userInput[0]) || isdigit(userInput[0]) || userInput[0] == '(' ||
			userInput.substr(0, 4) == "not(" || userInput.substr(0, 4) == "not " || userInput[0] == '#')
		{
			vector<string> tempcode = { userInput };
			lexanalysis.createPairs(tempcode);
			if (expEvaluation.toPostfix(lexanalysis.getTokens()[0]).empty())
			{
				cout << userInput << " is not recognized as a command" << endl << endl;
			}
			else
			{
				cout << expEvaluation.evalPostfix(expEvaluation.toPostfix(lexanalysis.getTokens()[0])) << endl;
				int linenum = 0;
				pysubi.evalLine(linenum, lexanalysis.getTokens(), expEvaluation);
			}
		}

		// invalid input
		else
			cout << userInput << " is not recognized as a command." << endl << endl;
	}
}

void Interface::quit()
{
	exit(0);
}

void Interface::clear()
{
	// clears the vector
	programCode.clear();

	cout << "Program cleared." << endl << endl;
}


void Interface::help(std::string command)
{
	// if skyscraper to find whatever command the user puts in
	if (command == "clear" || command == "clear()")
	{
		cout << "This command will clear whatever code you have loaded" << endl;
		cout << "into the program using the 'read' command." << endl;
	}
	else if (command == "help" || command == "help()")
	{
		cout << "This command will enter the help menu. Given a parameter, this command will" << endl;
		cout << "display the information for the given command without entering the help menu." << endl;
	}
	else if (command == "quit" || command == "quit()")
	{
		cout << "This command will quit the command line interpreter." << endl;
	}
	else if (command == "read" || command == "read()")
	{
		cout << "This command will read in any .py file into the interpreter." << endl;
		cout << "Requires a parameter in the form of read(<filename>.py)" << endl;
	}
	else if (command == "show" || command == "show()")
	{
		cout << "This command will display any of the code that has been loaded into the interpreter." << endl;
		cout << "If no code has been loaded using the read() command, then it will display an error." << endl;
	}
	else
	{
		cout << "That is not a command supported by the help utility." << endl;
	}
}

void Interface::help()
{
	// welcome message
	cout << "Welcome to the help utility!" << endl;
	cout << "* To exit and return to the interpreter, type 'exit'" << endl;
	cout << "* To get a list of commands, type 'commands'" << endl;
	cout << "* To get a description of any command, type the command name" << endl << endl;

	// loop continues until "quit" command
	while (1)
	{
		cout << "help> ";

		string helpInput;
		cin >> helpInput;

		// puts user input into lowercase
		for (int i = 0; i < helpInput.length(); i++)
			helpInput[i] = tolower(helpInput[i]);

		// same deal with the rest of the multitude of if statements
		// this is torture
		if (helpInput == "exit")
			break;
		else if (helpInput == "commands")
		{
			cout << "Below is a list of commands. Enter any command at the prompt to get more help." << endl << endl;
			cout << "clear\thelp\tquit" << endl;
			cout << "read\tshow" << endl << endl;
			cout << "NOTE: All commands can also be entered as functions:" << endl << endl;
			cout << "clear()\thelp()\tquit()" << endl;
			cout << "read()\tshow()" << endl << endl;
		}
		else if (helpInput == "clear" || helpInput == "clear()")
		{
			cout << "This command will clear whatever code you have loaded" << endl;
			cout << "into the program using the 'read' command." << endl;
		}
		else if (helpInput == "help" || helpInput == "help()")
		{
			cout << "This command will enter the help menu. Given a parameter, this command will" << endl;
			cout << "display the information for the given command without entering the help menu." << endl;
		}
		else if (helpInput == "quit" || helpInput == "quit()")
		{
			cout << "This command will quit the command line interpreter." << endl;
		}
		else if (helpInput == "read" || helpInput == "read()")
		{
			cout << "This command will read in any .py file into the interpreter." << endl;
			cout << "Requires a parameter in the form of read(<filename>.py)" << endl;
		}
		else if (helpInput == "show" || helpInput == "show()")
		{
			cout << "This command will display any of the code that has been loaded into the interpreter." << endl;
			cout << "If no code has been loaded using the read() command, then it will display an error." << endl;
		}
		else
		{
			cout << "That is not a command supported by the help utility." << endl;
		}
	}
}

bool Interface::read(std::string filename)
{
	// open file
	ifstream infile;
	infile.open(filename);

	// check if opened before doing anything
	if (infile.is_open())
	{
		clear();
		string tp;
		while (getline(infile, tp)) { //read data from file object and put it into the programCode vector
			programCode.push_back(tp);
		}
		infile.close();
		cout << "Program successfully read." << endl << endl;
		return true;
	}
	// error message if file could not be read
	else
	{
		cout << "Could not read file." << endl << endl;
		return false;
	}
}

void Interface::show()
{
	// error if the programCode vector is empty
	if (programCode.empty())
		cout << "You have not selected any file to be shown. Use the read() command first and try again." << endl << endl;
	// if not empty, just print by line
	// "[" << i << "] " syntax for line numbers
	else
	{
		int i = 1;
		for (auto a : programCode)
		{
			cout << "[" << i << "] " << a << endl;
			i++;
		}
		cout << endl;
	}
}