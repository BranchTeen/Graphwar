#pragma once
#include "Token.h"
#include <vector>

class ShuntingYard {
public:
    std::vector<Token> convert(const std::vector<Token> &tokens);
private:
    int precedence(const std::string &op) const;
    bool rightAssoc(const std::string &op) const;
};
