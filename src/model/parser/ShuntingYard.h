#pragma once
#include "Token.h"
#include <vector>

class ShuntingYard {
public:
    std::vector<Token> convert(const std::vector<Token> &tokens);
private:
    int precedence(const std::string &op) const;
    bool rightAssoc(const std::string &op) const;
    static bool isFunction(TokenType t);
    static bool isOperator(TokenType t);
    static std::string tokenToString(TokenType t);
};
