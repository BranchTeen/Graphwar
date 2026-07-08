#include "Token.h"
#include <QString>
#include <QVector>
#include <cctype>

static bool isFuncStart(const QString &s, int i, const QString &name) {
    if (i + name.length() > s.length()) return false;
    for (int j = 0; j < name.length(); ++j)
        if (s[i + j] != name[j]) return false;
    return true;
}

QVector<Token> tokenize(const QString &input) {
    QVector<Token> tokens;
    int i = 0;
    while (i < input.length()) {
        QChar c = input[i];
        if (c.isSpace()) { ++i; continue; }

        if (c.isDigit() || c == '.') {
            int start = i;
            while (i < input.length() && (input[i].isDigit() || input[i] == '.')) ++i;
            bool ok;
            double val = input.mid(start, i - start).toDouble(&ok);
            tokens.push_back({TokenType::Number, ok ? val : 0.0});
            continue;
        }

        if (c == 'x' || c == 'X') { tokens.push_back({TokenType::Variable}); ++i; continue; }
        if (c == '+') { tokens.push_back({TokenType::Plus}); ++i; continue; }
        if (c == '-') { tokens.push_back({TokenType::Minus}); ++i; continue; }
        if (c == '*') { tokens.push_back({TokenType::Multiply}); ++i; continue; }
        if (c == '/') { tokens.push_back({TokenType::Divide}); ++i; continue; }
        if (c == '^') { tokens.push_back({TokenType::Power}); ++i; continue; }
        if (c == '(') { tokens.push_back({TokenType::LParen}); ++i; continue; }
        if (c == ')') { tokens.push_back({TokenType::RParen}); ++i; continue; }

        struct FuncMap { QString name; TokenType type; };
        static const FuncMap funcs[] = {
            {"sin", TokenType::Sin}, {"cos", TokenType::Cos}, {"tan", TokenType::Tan},
            {"asin", TokenType::Asin}, {"acos", TokenType::Acos}, {"atan", TokenType::Atan},
            {"sqrt", TokenType::Sqrt}, {"abs", TokenType::Abs},
            {"log", TokenType::Log}, {"ln", TokenType::Ln}, {"exp", TokenType::Exp}
        };
        bool matched = false;
        for (const auto &f : funcs) {
            if (isFuncStart(input, i, f.name)) {
                tokens.push_back({f.type});
                i += f.name.length();
                matched = true;
                break;
            }
        }
        if (matched) continue;

        ++i; // skip unknown
    }
    tokens.push_back({TokenType::End});
    return tokens;
}
