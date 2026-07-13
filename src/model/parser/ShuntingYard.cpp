#include "ShuntingYard.h"
#include <stack>
#include <stdexcept>

int ShuntingYard::precedence(const std::string &op) const {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    if (op == "^") return 3;
    return 0;
}

bool ShuntingYard::rightAssoc(const std::string &op) const {
    return op == "^";
}

std::vector<Token> ShuntingYard::convert(const std::vector<Token> &tokens) {
    std::vector<Token> output;
    std::stack<Token> ops;

    for (const auto &tok : tokens) {
        switch (tok.type) {
        case TokenType::Number:
        case TokenType::Variable:
            output.push_back(tok);
            break;
        case TokenType::Function:
            ops.push(tok);
            break;
        case TokenType::Operator:
            while (!ops.empty() && ops.top().type != TokenType::LeftParen) {
                if (ops.top().type == TokenType::Operator &&
                    ((!rightAssoc(tok.text) && precedence(ops.top().text) >= precedence(tok.text)) ||
                     (rightAssoc(tok.text) && precedence(ops.top().text) > precedence(tok.text)))) {
                    output.push_back(ops.top());
                    ops.pop();
                } else break;
            }
            ops.push(tok);
            break;
        case TokenType::LeftParen:
            ops.push(tok);
            break;
        case TokenType::RightParen:
            while (!ops.empty() && ops.top().type != TokenType::LeftParen) {
                output.push_back(ops.top());
                ops.pop();
            }
            if (ops.empty()) throw std::runtime_error("Mismatched parentheses");
            ops.pop(); // remove left paren
            if (!ops.empty() && ops.top().type == TokenType::Function) {
                output.push_back(ops.top());
                ops.pop();
            }
            break;
        }
    }
    while (!ops.empty()) {
        if (ops.top().type == TokenType::LeftParen)
            throw std::runtime_error("Mismatched parentheses");
        output.push_back(ops.top());
        ops.pop();
    }
    return output;
}
