#pragma once

#include "lexanalyzer.h"
#include "expevaluator.h"
#include <stack>

class Interpreter
{
private:
	LexicalAnalyzer::tokenType code;
	std::stack<int> ifstack;
	std::stack<int> whilestack;
	bool ifencounter = false; // this is for verifying in "elif" and "else" if an "if" statement has come before 
	LexicalAnalyzer::tokenLineType whileexp;
	bool whileencounter = false; // this is for verifying if a "while" statement is currently being run or not during evalLine
	int jump = 0; // for use during while loop to jump back to start of while loop
public:
	bool evalLine(int&, const LexicalAnalyzer::tokenType, expEvaluator&);
	bool isIfEmpty();
	bool isWhileEmpty();
	void clearIf();
	void clearWhile();
};