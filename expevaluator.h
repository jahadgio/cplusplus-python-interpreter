#pragma once
#include <map>
#include <string>
#include "lexanalyzer.h"


class expEvaluator
{
private:
	typedef std::map<std::string, std::string> symbolTableType;
	symbolTableType symbolTable;
public:
	std::vector<std::pair<std::string, LexicalAnalyzer::categoryType>> toPostfix(LexicalAnalyzer::tokenLineType); // for one line, mostly for interpreter
	int evalPostfix(std::vector<std::pair<std::string, LexicalAnalyzer::categoryType>>);
	void toSymT(std::pair<std::string, std::string>);
	std::string fromSymT(std::string);
	void clearSymT();
	int getPrec(std::string);
	void showSymT();
};