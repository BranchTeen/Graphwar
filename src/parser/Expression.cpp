#include "Expression.h"
#include <cmath>
#include <algorithm>

double BinaryExpr::eval(double x) const {
    double l = left->eval(x), r = right->eval(x);
    switch (op) {
        case '+': return l + r;
        case '-': return l - r;
        case '*': return l * r;
        case '/': return l / r;
        case '^': return std::pow(l, r);
        default: return 0;
    }
}

int BinaryExpr::cost() const {
    int base = 0;
    switch (op) {
        case '+': case '-': base = 1; break;
        case '*': case '/': base = 2; break;
        case '^': base = 3; break;
    }
    return base + left->cost() + right->cost();
}

double UnaryExpr::eval(double x) const {
    double a = arg->eval(x);
    if (func == "sin")  return std::sin(a);
    if (func == "cos")  return std::cos(a);
    if (func == "tan")  return std::tan(a);
    if (func == "asin") return std::asin(std::clamp(a, -1.0, 1.0));
    if (func == "acos") return std::acos(std::clamp(a, -1.0, 1.0));
    if (func == "atan") return std::atan(a);
    if (func == "sqrt") return std::sqrt(std::fabs(a));
    if (func == "abs")  return std::fabs(a);
    if (func == "log")  return std::log10(std::fabs(a));
    if (func == "ln")   return std::log(std::fabs(a));
    if (func == "exp")  return std::exp(a);
    return 0;
}

int UnaryExpr::cost() const {
    int base = 0;
    if (func == "sqrt" || func == "abs") base = 2;
    else if (func == "sin" || func == "cos" || func == "tan" ||
             func == "log" || func == "ln"  || func == "exp") base = 3;
    else base = 4; // asin, acos, atan
    return base + arg->cost();
}
