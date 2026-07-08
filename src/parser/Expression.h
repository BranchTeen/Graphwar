#pragma once
#include <memory>
#include <QString>

struct Expr {
    virtual ~Expr() = default;
    virtual double eval(double x) const = 0;
    virtual int cost() const = 0;
};

struct NumberExpr : Expr {
    double value;
    explicit NumberExpr(double v) : value(v) {}
    double eval(double) const override { return value; }
    int cost() const override { return 1; }
};

struct VariableExpr : Expr {
    double eval(double x) const override { return x; }
    int cost() const override { return 1; }
};

struct BinaryExpr : Expr {
    char op;
    std::unique_ptr<Expr> left, right;
    BinaryExpr(char o, std::unique_ptr<Expr> l, std::unique_ptr<Expr> r)
        : op(o), left(std::move(l)), right(std::move(r)) {}
    double eval(double x) const override;
    int cost() const override;
};

struct UnaryExpr : Expr {
    QString func;
    std::unique_ptr<Expr> arg;
    UnaryExpr(const QString &f, std::unique_ptr<Expr> a)
        : func(f), arg(std::move(a)) {}
    double eval(double x) const override;
    int cost() const override;
};
