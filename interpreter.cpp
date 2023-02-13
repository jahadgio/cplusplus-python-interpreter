#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include "lexanalyzer.h"
#include "interpreter.h"

#define cat LexicalAnalyzer::categoryType

using namespace std;

// used as a predicate for removing quotes from a string literal
bool isQuotes(char c)
{
	switch (c)
	{
	case '\'':
	case '\"':
		return true;
	default:
		return false;
	}
}

string removequotes(string str)
{
	str.erase(remove_if(str.begin(), str.end(),
		&isQuotes), str.end()); // removes quotes from string literal << " ";
	return str;
}

bool Interpreter::evalLine(int& linenum, const LexicalAnalyzer::tokenType wholecode, expEvaluator& expeval)
{
	vector<pair<string, cat>> linetoeval = wholecode[linenum];

	for (int i = 0; i < linetoeval.size(); i++)
	{
		// *********************** PART OF EVALUATING "WHILE" *****************
		if ((whileencounter && linetoeval.size() > 0 && linetoeval[0].second != cat::INDENT) || (whileencounter && (linenum + 1 == wholecode.size())))
		{
			if (expeval.evalPostfix(expeval.toPostfix(whileexp)) == 1) // check while expression
			{
				linenum = jump;
				return true;
			}
			else
			{
				jump = 0;
				whileencounter = false;
				whileexp.clear();
			}
		}
		// *********************** COMMENTS ***********************
		if (linetoeval[i].second == cat::COMMENT)
		{
			return true; // ignoring comments
		}
		//************************ KEYWORDS ***********************
		else if (linetoeval[i].second == cat::KEYWORD)
		{
			// ********* PRINT *********
			if (linetoeval[i].first == "print")
			{
				// error checking for correct syntax with left parentheses
				if (i + 1 >= linetoeval.size())
				{
					cout << endl << "ERROR: Incorrect syntax at line " << linenum + 1 << endl;
					return false;
				}
				else if (linetoeval[i + 1].second != cat::LEFT_PAREN)
				{
					cout << endl << "ERROR: Missing left parentheses at line " << linenum + 1 << endl;
					return false;
				}
				else
				{
					vector<pair<string, cat>>::iterator it = linetoeval.begin() + i + 2; // iterator set to beginning of print statement 
					while (it != linetoeval.end())
					{
						vector<pair<string, cat>>::iterator expfinder = it;
						vector<pair<string, cat>> expression;
						while (expfinder != linetoeval.end() && (*expfinder).second != cat::COMMA)
						{
							if ((*expfinder).second == cat::RIGHT_PAREN && expfinder == linetoeval.end() - 1)
								break;
							expression.push_back(*expfinder);
							expfinder++;
						}
						it = expfinder; // update primary iterator to where the comma is
						// print out string literals right away
						if (expression[0].second == cat::STRING_LITERAL)
						{
							cout << removequotes(expression[0].first) << " ";
						}
						else if (expression[0].second == cat::LEFT_PAREN || expression[0].second == cat::NUMERIC_LITERAL)
						{
							auto temp = expeval.toPostfix(expression);
							if (temp.size() != 0)
							{
								cout << expeval.evalPostfix(temp) << " ";
							}
							// toPostfix returns empty vector if error, so check for error before printing
							else
							{
								return false;
							}
						}
						if (expression[0].second == cat::IDENTIFIER)
						{
							bool allnums = true;
							for (auto chars : expeval.fromSymT(expression[0].first))
							{
								if (!isdigit(chars))
								{
									allnums = false;
									break;
								}
							}
							if (allnums)
							{
								auto temp = expeval.toPostfix(expression);
								if (temp.size() != 0)
								{
									cout << expeval.evalPostfix(temp) << " ";
								}
								// toPostfix returns empty vector if error, so check for error before printing
								else
								{
									return false;
								}
							}
							else
							{
								cout << removequotes(expeval.fromSymT(expression[0].first)) << " ";
							}
						}
						if (it != linetoeval.end()) // avoid incrementing past end
							it++;
					}
					cout << endl;
				}
			}
			// ******** INT AND INPUT *********
			else if (linetoeval[i].first == "int")
			{
				// error checking for correct syntax with left parentheses
				if (i + 1 >= linetoeval.size() || linetoeval.size() != 9)
				{
					cout << endl << "ERROR: Incorrect syntax at line " << linenum + 1 << endl;
					return false;
				}
				else if (linetoeval[i + 1].second != cat::LEFT_PAREN)
				{
					cout << endl << "ERROR: Missing left parentheses at line " << linenum + 1 << endl;
					return false;
				}
				else
				{
					if (i + 2 >= linetoeval.size())
					{
						cout << endl << "ERROR: Incorrect syntax at line " << linenum + 1 << endl;
						return false;
					}
					else if (linetoeval[i + 2].first != "input")
					{
						cout << endl << "ERROR: Incorrect syntax at line " << linenum + 1 << endl;
						return false;
					}
					else
					{
						if (linetoeval[linetoeval.size() - 1].second != cat::RIGHT_PAREN && linetoeval[linetoeval.size() - 2].second != cat::RIGHT_PAREN)
						{
							cout << endl << "ERROR: Incorrect syntax at line " << linenum + 1 << endl;
							return false;
						}
						else
						{
							cout << removequotes(linetoeval[6].first);
							string leek;
							cin >> leek;
							cin.ignore();
							expeval.toSymT(make_pair(linetoeval[0].first, leek));
						}
					}
				}
			}
			// ************ IF ********
			else if (linetoeval[i].first == "if")
			{
				if (i + 2 < linetoeval.size() && linetoeval[linetoeval.size() - 1].second == cat::COLON)
				{
					// grab expression inside if statement and analyze it for truth
					LexicalAnalyzer::tokenLineType expression;
					for (int temp = i+1; temp < linetoeval.size() - 1; temp++)
					{
						expression.push_back(linetoeval[temp]);
					}
					// --------------------------------------------------
					ifencounter = true; // we have encountered an if statement
					// if true then execute code inside the if, otherwise we go to else and start skipping lines
					if (expeval.evalPostfix(expeval.toPostfix(expression)) == 1)
					{
						clearIf();
						ifstack.push(1);
					}
					else
					{
						// skipping over blocks of code only if they exist
						bool checkindent = false; // keep track if we ran into an indent, if we have not yet then the code has invalid syntax
						ifstack.push(0); // necessary for elif and else
						while (true)
						{
							if (linenum + 1 < wholecode.size() && wholecode[linenum+1][0].second == cat::INDENT) // if a next line exists and it begins with an indent...
							{
								checkindent = true;
								linenum = linenum + 1;
							}
							else if (checkindent == false)
							{
								cout << "ERROR: Incorrect syntax at line " << linenum+1 << endl;
								return false;
							}
							else
							{
								return true;
							}
						}
					}
				}
				else
				{
					cout << "ERROR: Incorrect syntax at line " << linenum+1 << endl;
					return false;
				}
			}

			// ************ ELIF ********
			else if (linetoeval[i].first == "elif")
			{
				if (i + 2 < linetoeval.size() && linetoeval[linetoeval.size() - 1].second == cat::COLON && ifencounter) // bounds checking, colon at the end of line
				{
					// grab expression inside if statement and analyze it for truth
					LexicalAnalyzer::tokenLineType expression;
					for (int temp = i + 1; temp < linetoeval.size() - 1; temp++)
					{
						expression.push_back(linetoeval[temp]);
					}
					// --------------------------------------------------

					// if true then execute code inside the if, otherwise we go to else and start skipping lines
					if (expeval.evalPostfix(expeval.toPostfix(expression)) == 1 && ifstack.top() == 0) // checking top of ifstack, if an if statement failed before then we can go ahead
					{
						clearIf();
						ifstack.push(1);
					}
					else
					{
						if(ifstack.top() == 0) // if the if statement before you failed and this also fails, then we will push a zero to the top of the stack
							ifstack.push(0);
						// skipping over blocks of code only if they exist
						bool checkindent = false; // keep track if we ran into an indent, if we have not yet then the code has invalid syntax
						while (true)
						{
							if (linenum + 1 < wholecode.size() && wholecode[linenum+1].size() > 0 && wholecode[linenum + 1][0].second == cat::INDENT) // if a next line exists and it begins with an indent...
							{
								checkindent = true;
								linenum = linenum + 1;
							}
							else if (checkindent == false)
							{
								cout << "ERROR: Incorrect syntax at line " << linenum+1 << endl;
								return false;
							}
							else
							{
								return true;
							}
						}
					}
				}
				else
				{
					cout << "ERROR: Incorrect syntax at line " << linenum+1 << endl;
					return false;
				}
			}

			// ********** ELSE **********
			else if (linetoeval[i].first == "else")
			{
				if (i + 1 < linetoeval.size() && linetoeval[linetoeval.size() - 1].second == cat::COLON && ifencounter) // bounds checking, colon at the end of line, make sure an if was encountered
				{
					// if true then execute code inside the else, otherwise we go to else and start skipping lines
					if (ifstack.top() == 0) // checking top of ifstack, if an if statement failed before then we can go ahead
					{
						clearIf();
						ifencounter = false; // reset ifencounter for future encounters
					}
					else
					{
						// skipping over blocks of code only if they exist
						bool checkindent = false; // keep track if we ran into an indent, if we have not yet then the code has invalid syntax
						ifstack.push(0);
						while (true)
						{
							if (linenum + 1 < wholecode.size() && wholecode[linenum + 1].size() > 0 && wholecode[linenum + 1][0].second == cat::INDENT) // if a next line exists and it begins with an indent...
							{
								checkindent = true;
								linenum = linenum + 1;
							}
							else if (checkindent == false)
							{
								cout << "ERROR: Incorrect syntax at line " << linenum + 1 << endl;
								return false;
							}
							else
							{
								return true;
							}
						}
					}
				}
				else
				{
					cout << "ERROR: Incorrect syntax at line " << linenum + 1 << endl;
					return false;
				}
			}

			// ************ WHILE ********
			else if (linetoeval[i].first == "while")
			{
				if (i + 2 < linetoeval.size() && linetoeval[linetoeval.size() - 1].second == cat::COLON)
				{
					// grab expression inside while statement and analyze it for truth
					for (int temp = i + 1; temp < linetoeval.size() - 1; temp++)
					{
						whileexp.push_back(linetoeval[temp]);
					}
					// --------------------------------------------------
					// if true then execute code inside the while, otherwise skip
					if (expeval.evalPostfix(expeval.toPostfix(whileexp)) == 1)
					{
						whileencounter = true;
						jump = linenum;
						return true;
					}
					else
					{
						// skipping over blocks of code only if they exist
						bool checkindent = false; // keep track if we ran into an indent, if we have not yet then the code has invalid syntax
						while (true)
						{
							if (linenum + 1 < wholecode.size() && wholecode[linenum + 1][0].second == cat::INDENT) // if a next line exists and it begins with an indent...
							{
								checkindent = true;
								linenum = linenum + 1;
							}
							else if (checkindent == false)
							{
								cout << "ERROR: Incorrect syntax at line " << linenum + 1 << endl;
								return false;
							}
							else
							{
								return true;
							}
						}
					}
				}
				else
				{
					cout << "ERROR: Incorrect syntax at line " << linenum + 1 << endl;
					return false;
				}
			}
		}

		// ******************* IDENTIFIERS ***********************
		// note: printing identifiers is handled under print
		else if (linetoeval[i].second == cat::ASSIGNMENT_OP)
		{
			if (i != 0 && linetoeval[i - 1].second == cat::IDENTIFIER && i + 1 < linetoeval.size() && (linetoeval[i + 1].second == cat::LEFT_PAREN ||
				linetoeval[i + 1].second == cat::KEYWORD || linetoeval[i + 1].second == cat::NUMERIC_LITERAL ||linetoeval[i+1].second == cat::IDENTIFIER ||
				linetoeval[i + 1].second == cat::STRING_LITERAL)) // though horrific to look at, this is checking if to the left and right of an assignment op if there is a 
																  // valid identifier to assign to on the left and somethign to assign to it on the right, bounds checking included
			{
				if (linetoeval[i + 1].second == cat::STRING_LITERAL)
				{
					expeval.toSymT(make_pair(linetoeval[i - 1].first, linetoeval[i + 1].first));
				}
				else if (linetoeval[i + 1].second == cat::NUMERIC_LITERAL || linetoeval[i + 1].second == cat::LEFT_PAREN || linetoeval[i + 1].second == cat::IDENTIFIER)
				{
					string temp = to_string(expeval.evalPostfix(expeval.toPostfix(linetoeval)));
					expeval.toSymT(make_pair(linetoeval[i - 1].first, temp));
				}
			}
		}
	}
	return true;
}

bool Interpreter::isIfEmpty()
{
	if (ifstack.empty())
		return true;
	return false;
}
bool Interpreter::isWhileEmpty()
{
	if (whilestack.empty())
		return true;
	return false;
}

void Interpreter::clearIf()
{
	while (!ifstack.empty())
		ifstack.pop();
}
void Interpreter::clearWhile()
{
	while (!whilestack.empty())
		whilestack.pop();
}