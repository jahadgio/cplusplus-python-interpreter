#include "lexanalyzer.h"
#include <vector>
#include <string>
#include <iostream>
#include <typeinfo>
#include <ctype.h>
#include <map>
#include <stack>

using namespace std;

void LexicalAnalyzer::clear()
{
	tokenInfo.clear();
}

// function to create tokens, each token has a string in the first and a categoryType in the second
void LexicalAnalyzer::createPairs(vector<string> glarp)
{
	clear();
	tokenInfo.resize(glarp.size()); // make the size tokenInfo equal to the number of lines in the program

	
	for (int i = 0; i < glarp.size(); i++)
	{
		bool encounter = false; // tracks if we have encountered a character in the line -- if we have then we ignore spaces
		int strloc = 0; // when finding something, using this to track the index in the string of whatever is found so that...
		int strlocend = 0; // ... we can then use this for .substr to capture whatever string we want
		for (int j = 0; j < glarp[i].size(); j++) // doing everything inside this loop
		{
			strlocend = 0; // for .susbtr count

			// to check if at the beginning of a line, if not then ignore within the loop
			if (glarp[i][j] != ' ' && glarp[i][j] != '\t')
			{
				encounter = true;
			}
			// indents
			if (glarp[i][j] == ' ' && encounter == false || glarp[i][j] == '\t' && encounter == false)
			{
				strloc = j; // beginning of substr
				while (glarp[i][j] == ' ' || glarp[i][j] == '\t') // keep counting while we find spaces
				{
					strlocend++;
					j++;
				}
				j--; // so we don't skip a character after the loop updates
				string temp = glarp[i].substr(strloc, strlocend);
				tokenInfo[i].push_back(make_pair(temp, categoryType::INDENT));
				encounter = true; // once we find a letter ignore the rest of the spaces in the line
			}

			// numeric literals
			else if (isdigit(glarp[i][j]))
			{
				strloc = j;
				while (isdigit(glarp[i][j]))
				{
					strlocend++;
					j++;
				}
				j--; // so we do not skip characters after loop updates
				string temp = glarp[i].substr(strloc, strlocend);
				tokenInfo[i].push_back(make_pair(temp, categoryType::NUMERIC_LITERAL));
			}

			// letters including operators
			else if (isalpha(glarp[i][j]))
			{
				strloc = j;
				while (isalpha(glarp[i][j]))
				{
					strlocend++;
					j++;
				}
				j--; // so we do not skip characters after loop updates
				string temp = glarp[i].substr(strloc, strlocend);
				// check for logical operators
				if (temp == "and" || temp == "or" || temp == "not")
				{
					tokenInfo[i].push_back(make_pair(temp, categoryType::LOGICAL_OP));
				}
				// check for keywords
				else if (temp == "if" || temp == "elif" || temp == "else" || temp == "print" || temp == "input" || temp == "int" || temp == "while")
				{
					tokenInfo[i].push_back(make_pair(temp, categoryType::KEYWORD));
				}
				// if none, then it's an identifier
				else
				{
					tokenInfo[i].push_back(make_pair(temp, categoryType::IDENTIFIER));
				}

			}

			// string literals
			else if (glarp[i][j] == '\'') // single quotes
			{
				bool endquotes = true; // set to false if we cannot find a matching quote
				strloc = j+1; // j+1 for out of bounds checking
				while (glarp[i][j + 1] != '\'')
				{
					// out of bounds check for quotes with no end quote
					if (j >= glarp[i].length() - 1)
					{
						string temp = glarp[i].substr(strloc, strlocend);
						tokenInfo[i].push_back(make_pair(glarp[i][j] + temp, categoryType::UNKNOWN));
						endquotes = false;
						break;
					}

					strlocend++;
					j++;
				}
				if (endquotes == true) // only assign string if there is a matching quote
				{
					string temp = glarp[i].substr(strloc, strlocend);
					tokenInfo[i].push_back(make_pair('\'' + temp + '\'', categoryType::STRING_LITERAL));
					j++;
				}
			}
			else if (glarp[i][j] == '"') // double quotes
			{
				strloc = j+1; // j+1 for out of bounds checking
				bool endquotes = true; // set to false if we cannot find a matching quote
				while (glarp[i][j + 1] != '"')
				{
					// out of bounds check for quotes with no end quote
					if (j >= glarp[i].length() - 1)
					{
						string temp = glarp[i].substr(strloc, strlocend);
						tokenInfo[i].push_back(make_pair(glarp[i][j] + temp, categoryType::UNKNOWN));
						endquotes = false;
						break;
					}

					strlocend++;
					j++;
				}
				if (endquotes == true) // only assign string if there is a matching quote
				{
					string temp = glarp[i].substr(strloc, strlocend);
					tokenInfo[i].push_back(make_pair('"' + temp + '"', categoryType::STRING_LITERAL));
					j++;
				}
			}

			// parentheses, colon, comma
			else if (glarp[i][j] == '(' || glarp[i][j] == ')' || glarp[i][j] == ':' || glarp[i][j] == ',')
			{
				if (glarp[i][j] == '(')
				{
					tokenInfo[i].push_back(make_pair(string("("), categoryType::LEFT_PAREN));
				}
				if (glarp[i][j] == ')')
				{
					tokenInfo[i].push_back(make_pair(string(")"), categoryType::RIGHT_PAREN));
				}
				if (glarp[i][j] == ':')
				{
					tokenInfo[i].push_back(make_pair(string(":"), categoryType::COLON));
				}
				if (glarp[i][j] == ',')
				{
					tokenInfo[i].push_back(make_pair(string(","), categoryType::COMMA));
				}
			}

			// comments
			else if (glarp[i][j] == '#')
			{
				tokenInfo[i].push_back(make_pair(glarp[i], categoryType::COMMENT));
				break; // resets back to primary loop
			}

			// relational ops and assignment ops
			else if (glarp[i][j] == '<' || glarp[i][j] == '>' || glarp[i][j] == '=' || glarp[i][j] == '!')
			{
				if (glarp[i][j] == '<' || glarp[i][j] == '>') // check greater or less than ops
				{
					if ((j+1) < glarp[i].length() && glarp[i][j+1] == '=')
					{
						tokenInfo[i].push_back(make_pair(glarp[i].substr(j, 2), categoryType::RELATIONAL_OP));
						j++; // move the loop forward one since we are checking ahead of the loop's current character
					}
					else
					{
						tokenInfo[i].push_back(make_pair(string(1, glarp[i][j]), categoryType::RELATIONAL_OP));
					}
				}
				else if (glarp[i][j] == '=') // check equals signs
				{
					if ((j + 1) < glarp[i].length() && glarp[i][j + 1] == '=')
					{
						tokenInfo[i].push_back(make_pair(glarp[i].substr(j, 2), categoryType::RELATIONAL_OP));
						j++; // move the loop forward one since we are checking ahead of the loop's current character
					}
					else // if nothing was found after equals, it's an assignment op
					{
						tokenInfo[i].push_back(make_pair(glarp[i].substr(j, 1), categoryType::ASSIGNMENT_OP));
					}
				}
				else if (glarp[i][j] == '!') // check exclamation marks for not equals
				{
					if ((j + 1) < glarp[i].length() && glarp[i][j + 1] == '=')
					{
						tokenInfo[i].push_back(make_pair(glarp[i].substr(j, 2), categoryType::RELATIONAL_OP));
						j++; // move the loop forward one since we are checking ahead of the loop's current character
					}
				}
			}

			// arithmetic ops
			else if (glarp[i][j] == '+' || glarp[i][j] == '-' || glarp[i][j] == '*' || glarp[i][j] == '/' || glarp[i][j] == '%')
			{
			tokenInfo[i].push_back(make_pair(glarp[i].substr(j, 1), categoryType::ARITH_OP));
			}
		}
	}
	tokenLineType helpme;
	helpme.push_back(make_pair("", categoryType::UNKNOWN));
	tokenInfo.push_back(helpme);
	
}

void LexicalAnalyzer::showTokens()
{
	if (tokenInfo.empty())
	{
		cout << "You have not selected any file to be shown. Use the read() command first and try again." << endl << endl;
	}
	map<categoryType, string> types; // map for printing categoryType

	// ***************************************************************************
	types.insert(pair<categoryType, string>(categoryType::KEYWORD, "KEYWORD"));
	types.insert(pair<categoryType, string>(categoryType::IDENTIFIER, "IDENTIFIER"));
	types.insert(pair<categoryType, string>(categoryType::STRING_LITERAL, "STRING_LITERAL"));
	types.insert(pair<categoryType, string>(categoryType::NUMERIC_LITERAL, "NUMERIC_LITERAL"));
	types.insert(pair<categoryType, string>(categoryType::ASSIGNMENT_OP, "ASSIGNMENT_OP"));
	types.insert(pair<categoryType, string>(categoryType::ARITH_OP, "ARITH_OP"));
	types.insert(pair<categoryType, string>(categoryType::LOGICAL_OP, "LOGICAL_OP"));
	types.insert(pair<categoryType, string>(categoryType::RELATIONAL_OP, "RELATIONAL_OP"));
	types.insert(pair<categoryType, string>(categoryType::LEFT_PAREN, "LEFT_PAREN"));
	types.insert(pair<categoryType, string>(categoryType::RIGHT_PAREN, "RIGHT_PAREN"));
	types.insert(pair<categoryType, string>(categoryType::COLON, "COLON"));
	types.insert(pair<categoryType, string>(categoryType::COMMA, "COMMA"));
	types.insert(pair<categoryType, string>(categoryType::COMMENT, "COMMENT"));
	types.insert(pair<categoryType, string>(categoryType::INDENT, "INDENT"));
	types.insert(pair<categoryType, string>(categoryType::UNKNOWN, "UNKNOWN"));
	// ****************************************************************************
	// print token info
	for (int i = 0; i < tokenInfo.size()-1; i++)
	{
		cout << "Line #" << i << ":" << endl;
		for (int j = 0; j < tokenInfo[i].size(); j++)
		{
			cout << "Token[" << j << "]:\t";
			cout << tokenInfo[i][j].first << " - ";
			cout << types.find(tokenInfo[i][j].second)->second << endl; // find map string pair to print
		}
		cout << "---------------------------------------------------" << endl;
	}
}

LexicalAnalyzer::tokenType LexicalAnalyzer::getTokens()
{
	return tokenInfo;
}
