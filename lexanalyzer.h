#pragma once

#include <vector>
#include <string>

class LexicalAnalyzer
{
private:
    enum class categoryType
    {
        KEYWORD,
        IDENTIFIER,
        STRING_LITERAL,
        NUMERIC_LITERAL,
        ASSIGNMENT_OP,
        ARITH_OP,
        LOGICAL_OP,
        RELATIONAL_OP,
        LEFT_PAREN,
        RIGHT_PAREN,
        COLON,
        COMMA,
        COMMENT,
        INDENT,
        UNKNOWN
    };

    typedef std::vector<std::vector<std::pair<std::string, categoryType>>> tokenType;
    typedef std::vector<std::pair<std::string, categoryType>> tokenLineType;
    typedef std::pair<std::string, categoryType> pairType;
    tokenType tokenInfo;

    friend class expEvaluator;
    friend class Interpreter;

public:
    void createPairs(std::vector<std::string> glarp);
    void showTokens();
    void clear();
    tokenType getTokens();
};