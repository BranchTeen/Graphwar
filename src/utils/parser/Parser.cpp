#include "Token.h"
#include "Expression.h"
#include <QVector>
#include <memory>

class Parser {
    const QVector<Token> &tokens;
    int pos = 0;

    Token peek() const { return tokens[pos]; }
    Token advance() { return tokens[pos++]; }
    bool match(TokenType t) {
        if (peek().type == t) { advance(); return true; }
        return false;
    }

    std::unique_ptr<Expr> parseExpr() {
        auto left = parseTerm();
        while (peek().type == TokenType::Plus || peek().type == TokenType::Minus) {
            char op = (advance().type == TokenType::Plus) ? '+' : '-';
            auto right = parseTerm();
            left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
        }
        return left;
    }

    std::unique_ptr<Expr> parseTerm() {
        auto left = parsePower();
        while (peek().type == TokenType::Multiply || peek().type == TokenType::Divide) {
            char op = (advance().type == TokenType::Multiply) ? '*' : '/';
            auto right = parsePower();
            left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
        }
        return left;
    }

    std::unique_ptr<Expr> parsePower() {
        auto left = parseUnary();
        if (peek().type == TokenType::Power) {
            advance();
            auto right = parsePower(); // right-associative
            left = std::make_unique<BinaryExpr>('^', std::move(left), std::move(right));
        }
        return left;
    }

    std::unique_ptr<Expr> parseUnary() {
        if (peek().type == TokenType::Plus) {
            advance();
            return parseUnary();
        }
        if (peek().type == TokenType::Minus) {
            advance();
            auto arg = parseUnary();
            // 负号（一元减）不消耗额外点数，等价于取负
            return std::make_unique<NegateExpr>(std::move(arg));
        }
        return parsePostfix();
    }

    std::unique_ptr<Expr> parsePostfix() {
        if (peek().type == TokenType::LParen) {
            advance();
            auto expr = parseExpr();
            match(TokenType::RParen);
            return expr;
        }
        if (peek().type == TokenType::Number) {
            return std::make_unique<NumberExpr>(advance().value);
        }
        if (peek().type == TokenType::Variable) {
            advance();
            return std::make_unique<VariableExpr>();
        }
        // Function call: sin(x), cos(x+2), etc.
        static const TokenType funcs[] = {
            TokenType::Sin, TokenType::Cos, TokenType::Tan,
            TokenType::Asin, TokenType::Acos, TokenType::Atan,
            TokenType::Sqrt, TokenType::Abs,
            TokenType::Log, TokenType::Ln, TokenType::Exp
        };
        for (auto ft : funcs) {
            if (peek().type == ft) {
                QString name = Token::typeName(ft);
                advance();
                if (peek().type == TokenType::LParen) advance();
                auto arg = parseExpr();
                if (peek().type == TokenType::RParen) advance();
                return std::make_unique<UnaryExpr>(name, std::move(arg));
            }
        }
        return std::make_unique<NumberExpr>(0);
    }

public:
    explicit Parser(const QVector<Token> &t) : tokens(t) {}

    std::unique_ptr<Expr> parse() {
        auto expr = parseExpr();
        return expr;
    }
};

std::unique_ptr<Expr> parseExpression(const QVector<Token> &tokens) {
    Parser parser(tokens);
    return parser.parse();
}
