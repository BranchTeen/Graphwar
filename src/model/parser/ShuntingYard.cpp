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

bool ShuntingYard::isFunction(TokenType t) {
    switch (t) {
        case TokenType::Sin: case TokenType::Cos: case TokenType::Tan:
        case TokenType::Asin: case TokenType::Acos: case TokenType::Atan:
        case TokenType::Sqrt: case TokenType::Abs:
        case TokenType::Log: case TokenType::Ln: case TokenType::Exp:
            return true;
        default: return false;
    }
}

bool ShuntingYard::isOperator(TokenType t) {
    switch (t) {
        case TokenType::Plus: case TokenType::Minus:
        case TokenType::Multiply: case TokenType::Divide:
        case TokenType::Power:
            return true;
        default: return false;
    }
}

std::string ShuntingYard::tokenToString(TokenType t) {
    switch (t) {
        case TokenType::Plus:     return "+";
        case TokenType::Minus:    return "-";
        case TokenType::Multiply: return "*";
        case TokenType::Divide:   return "/";
        case TokenType::Power:    return "^";
        case TokenType::Sin:      return "sin";
        case TokenType::Cos:      return "cos";
        case TokenType::Tan:      return "tan";
        case TokenType::Asin:     return "asin";
        case TokenType::Acos:     return "acos";
        case TokenType::Atan:     return "atan";
        case TokenType::Sqrt:     return "sqrt";
        case TokenType::Abs:      return "abs";
        case TokenType::Log:      return "log";
        case TokenType::Ln:       return "ln";
        case TokenType::Exp:      return "exp";
        default: return "";
    }
}

std::vector<Token> ShuntingYard::convert(const std::vector<Token> &tokens) {
    std::vector<Token> output;
    std::stack<Token> ops;

    for (const auto &tok : tokens) {
        if (tok.type == TokenType::Number || tok.type == TokenType::Variable) {
            output.push_back(tok);
            continue;
        }

        if (isFunction(tok.type)) {
            ops.push(tok);
            continue;
        }

        if (isOperator(tok.type)) {
            while (!ops.empty() && ops.top().type != TokenType::LParen) {
                if (isOperator(ops.top().type)) {
                    std::string topOp = tokenToString(ops.top().type);
                    std::string currOp = tokenToString(tok.type);
                    if ((!rightAssoc(currOp) && precedence(topOp) >= precedence(currOp)) ||
                        (rightAssoc(currOp) && precedence(topOp) > precedence(currOp))) {
                        output.push_back(ops.top());
                        ops.pop();
                    } else break;
                } else break;
            }
            ops.push(tok);
            continue;
        }

        if (tok.type == TokenType::LParen) {
            ops.push(tok);
            continue;
        }

        if (tok.type == TokenType::RParen) {
            while (!ops.empty() && ops.top().type != TokenType::LParen) {
                output.push_back(ops.top());
                ops.pop();
            }
            if (ops.empty()) throw std::runtime_error("Mismatched parentheses");
            ops.pop();
            if (!ops.empty() && isFunction(ops.top().type)) {
                output.push_back(ops.top());
                ops.pop();
            }
            continue;
        }
    }

    while (!ops.empty()) {
        if (ops.top().type == TokenType::LParen)
            throw std::runtime_error("Mismatched parentheses");
        output.push_back(ops.top());
        ops.pop();
    }

    return output;
}
