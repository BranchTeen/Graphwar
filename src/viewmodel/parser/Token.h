#pragma once
#include <QString>

enum class TokenType {
    Number,
    Variable,
    Plus,
    Minus,
    Multiply,
    Divide,
    Power,
    Sin, Cos, Tan,
    Asin, Acos, Atan,
    Sqrt, Abs,
    Log, Ln, Exp,
    LParen,
    RParen,
    End,
    Unknown
};

struct Token {
    TokenType type = TokenType::Unknown;
    double value = 0.0;

    static QString typeName(TokenType t) {
        switch (t) {
            case TokenType::Number:   return "Number";
            case TokenType::Variable:  return "x";
            case TokenType::Plus:      return "+";
            case TokenType::Minus:     return "-";
            case TokenType::Multiply:  return "*";
            case TokenType::Divide:    return "/";
            case TokenType::Power:     return "^";
            case TokenType::Sin:       return "sin";
            case TokenType::Cos:       return "cos";
            case TokenType::Tan:       return "tan";
            case TokenType::Asin:      return "asin";
            case TokenType::Acos:      return "acos";
            case TokenType::Atan:      return "atan";
            case TokenType::Sqrt:      return "sqrt";
            case TokenType::Abs:       return "abs";
            case TokenType::Log:       return "log";
            case TokenType::Ln:        return "ln";
            case TokenType::Exp:       return "exp";
            case TokenType::LParen:    return "(";
            case TokenType::RParen:    return ")";
            default: return "?";
        }
    }
};
