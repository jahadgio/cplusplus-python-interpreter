#include <string>
#include <map>
#include <iostream>
#include <stack>
#include "expevaluator.h"
#include "lexanalyzer.h"

#define cat LexicalAnalyzer::categoryType

using namespace std;
vector<pair<string, LexicalAnalyzer::categoryType>> expEvaluator::toPostfix(LexicalAnalyzer::tokenLineType arg)
{
	// postfix vect for result to be returned at end
	// stackvect to simulate stack
	vector<pair<string, LexicalAnalyzer::categoryType>> postfixVect;
	vector<pair<string, LexicalAnalyzer::categoryType>> stackVect;

	// going through input vector
	for (int i = 0; i < arg.size(); i++)
	{
			auto current = arg[i]; // current token
			switch (arg[i].second) { // taking cases based on category type
			case cat::LEFT_PAREN:
				stackVect.push_back(current);
				break;
			case cat::RIGHT_PAREN:
				if (stackVect.empty()) // breaks out of loop, clears postfixVect, and displays error message if reached
				{
					goto endloop;
				}
				while (true) // loops until reaching an open parentheses or errors
				{
					if (stackVect.empty()) // check if empty before going further, ensures that we will have a matching left paren or will exit loop
					{
						goto endloop;
					}
					if (stackVect.back().first == "(")
					{
						stackVect.pop_back();
						break;
					}
					postfixVect.push_back(stackVect.back());
					stackVect.pop_back();
				}
				break;
			case cat::ARITH_OP:
			case cat::RELATIONAL_OP:
			case cat::LOGICAL_OP:
				if (i == arg.size() - 1) // error if the last thing in arg is an operator
				{
					cout << "ERROR: Incorrect syntax, operator at end of input";
					exit(1);
				}
				// ensures that we don't have two operators in a row
				if (arg[i + 1].second == cat::ARITH_OP || arg[i + 1].second == cat::RELATIONAL_OP || arg[i + 1].second == cat::LOGICAL_OP && arg[i + 1].first != "not")
				{
					cout << "ERROR: Incorrect syntax, unmatched operator";
					exit(1);
				}
				while (!stackVect.empty() && stackVect.back().first != "(" &&
					getPrec(stackVect.back().first) >= getPrec(current.first)) // getPrec gets operator precedence
				{
					postfixVect.push_back(stackVect.back());
					stackVect.pop_back();
				}
				stackVect.push_back(current);
				break;
			case cat::IDENTIFIER:
					if (symbolTable.find(current.first)!=symbolTable.end())
					{
						bool allnums = true;
						for (auto chars : symbolTable.find(current.first)->second)
						{ 
							if (!isdigit(chars)) 
							{ 
								allnums = false;
								break;
							}
						}

						// if the input is all numbers then assign it to a numeric literal, otherwise do nothing -- it does not factor
						// into the postfix expression
						if(allnums)
							postfixVect.push_back(make_pair(symbolTable.find(current.first)->second, cat::NUMERIC_LITERAL));
					}
					else if (i + 1 != arg.size() && arg[i+1].second == cat::ASSIGNMENT_OP)
					{
						continue; // if the identifier is being assigned to something (an expression), then we don't want it to reject it as an error
					}
					else
					{
						cout << endl << "ERROR: No value found for key=" << current.first << endl;
						postfixVect.clear();
						return postfixVect;
					}
					break;
			case cat::NUMERIC_LITERAL:
					postfixVect.push_back(current);
					break;
			}

	}

	while (!stackVect.empty())
	{
		if (stackVect.back().first == "(") // check if there is an unmatched left parentheses
		{
			cout << "ERROR: Unmatched left parentheses. Unable to evaluate." << endl;
			postfixVect.clear();
			exit(1);
		}
		postfixVect.push_back(stackVect.back());
		stackVect.pop_back();
	}

	return postfixVect;

endloop:
	cout << "ERROR: Unmatched right parentheses. Unable to evaluate." << endl;
	exit(1);
}

int expEvaluator::evalPostfix(vector<pair<string, LexicalAnalyzer::categoryType>> arg)
{
	double result = 0;
	stack<double> tokenstack;

	// just a whole bunch of different operators to find out what operation to do
	for (auto i : arg)
	{
		if (i.second == cat::NUMERIC_LITERAL)
		{
			tokenstack.push(stoi(i.first));
		}
		else if (i.first == "*")
		{
			double operand2 = tokenstack.top();
			tokenstack.pop();
			double operand1 = tokenstack.top();
			tokenstack.pop();
			tokenstack.push(operand1 * operand2);
		}
		else if (i.first == "/")
		{
			double operand2 = tokenstack.top();
			tokenstack.pop();
			double operand1 = tokenstack.top();
			tokenstack.pop();
			tokenstack.push(operand1 / operand2);
		}
		else if (i.first == "%")
		{
			double operand2 = tokenstack.top();
			tokenstack.pop();
			double operand1 = tokenstack.top();
			tokenstack.pop();
			tokenstack.push(int(operand1) % int(operand2));
		} 
		else if (i.first == "+")
		{
			double operand2 = tokenstack.top();
			tokenstack.pop();
 			double operand1 = tokenstack.top();
			tokenstack.pop();
			tokenstack.push(operand1 + operand2);
		}
		else if (i.first == "-")
		{
			double operand2 = tokenstack.top();
			tokenstack.pop();
			double operand1 = tokenstack.top();
			tokenstack.pop();
			tokenstack.push(operand1 - operand2);
		}
		else if (i.first == "<")
		{
			double operand2 = tokenstack.top();
			tokenstack.pop();
			double operand1 = tokenstack.top();
			tokenstack.pop();
			tokenstack.push(operand1 < operand2);
		}
		else if (i.first == "<=")
		{
			double operand2 = tokenstack.top();
			tokenstack.pop();
			double operand1 = tokenstack.top();
			tokenstack.pop();
			tokenstack.push(operand1 <= operand2);
		}
		else if (i.first == ">")
		{
			double operand2 = tokenstack.top();
			tokenstack.pop();
			double operand1 = tokenstack.top();
			tokenstack.pop();
			tokenstack.push(operand1 > operand2);
		}
		else if (i.first == ">=")
		{
			double operand2 = tokenstack.top();
			tokenstack.pop();
			double operand1 = tokenstack.top();
			tokenstack.pop();
			tokenstack.push(operand1 >= operand2);
		}
		else if (i.first == "!=")
		{
			double operand2 = tokenstack.top();
			tokenstack.pop();
			double operand1 = tokenstack.top();
			tokenstack.pop();
			tokenstack.push(operand1 != operand2);
		}
		else if (i.first == "==")
		{
			double operand2 = tokenstack.top();
			tokenstack.pop();
			double operand1 = tokenstack.top();
			tokenstack.pop();
			tokenstack.push(operand1 == operand2);
		}
		else if (i.first == "not")
		{
			double operand1 = tokenstack.top();
			tokenstack.pop();
			tokenstack.push(!operand1);
		
		} 
		else if (i.first == "and")
		{
			double operand2 = tokenstack.top();
			tokenstack.pop();
			double operand1 = tokenstack.top();
			tokenstack.pop();
			tokenstack.push(operand1 && operand2);
		}
		else if (i.first == "or")
		{
			double operand2 = tokenstack.top();
			tokenstack.pop();
			double operand1 = tokenstack.top();
			tokenstack.pop();
			tokenstack.push(operand1 || operand2);
		}
	}
	result = tokenstack.top();
	return result;
}

void expEvaluator::toSymT(pair<string, string> symbol) // write to symbolTable using a pair of strings
{
	if(symbolTable.find(symbol.first) == symbolTable.end())
		symbolTable.insert(symbol);
	else
	{
		symbolTable.erase(symbol.first);
		symbolTable.insert(symbol);
	}
}

string expEvaluator::fromSymT(string key) // get from symbolTable using a key string, returns empty string if none found
{
	if (symbolTable.find(key) != symbolTable.end())
	{
		return symbolTable.find(key)->second;
	}
	else
	{
		cout << endl << "ERROR: No key-element pair found for key=" << key << endl;
		return "";
	}
}

void expEvaluator::clearSymT() // clears symbolTable
{
	symbolTable.clear();
}

int expEvaluator::getPrec(string input)
{
	map<string, int> precedence; // operator-precedence pairs
	// ******************************************************************
	precedence.insert(pair<string, int>("*", 5));
	precedence.insert(pair<string, int>("/", 5));
	precedence.insert(pair<string, int>("%", 5));
	precedence.insert(pair<string, int>("+", 4));
	precedence.insert(pair<string, int>("-", 4));
	precedence.insert(pair<string, int>("<", 3));
	precedence.insert(pair<string, int>("<=", 3));
	precedence.insert(pair<string, int>(">", 3));
	precedence.insert(pair<string, int>(">=", 3));
	precedence.insert(pair<string, int>("!=", 3));
	precedence.insert(pair<string, int>("==", 3));
	precedence.insert(pair<string, int>("not", 2));
	precedence.insert(pair<string, int>("and", 1));
	precedence.insert(pair<string, int>("or", 0));
	// ******************************************************************

	return precedence.find(input)->second;
}

void expEvaluator::showSymT()
{
	if (symbolTable.empty())
	{
		cout << endl << "You have not selected any program to be read, or you have not run your program yet." << endl << endl;
	}
	else
	{
		cout << "******************Symbol Table Variables and Values********************" << endl;
		for (auto all : symbolTable)
		{
			cout << all.first << " = " << all.second << endl;
		}
		cout << endl;
	}
}