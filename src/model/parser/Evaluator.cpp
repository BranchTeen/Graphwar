#include "Tokenizer.h"
#include "Parser.h"

double evaluate(const QString &input, double x) {
    auto tokens = tokenize(input);
    auto expr = parseExpression(tokens);
    if (!expr) return 0;
    return expr->eval(x);
}

int calculateCost(const QString &input) {
    auto tokens = tokenize(input);
    auto expr = parseExpression(tokens);
    if (!expr) return 0;
    return expr->cost();
}
