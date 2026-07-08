#pragma once
#include <memory>
#include <QVector>
#include "Token.h"
#include "Expression.h"

std::unique_ptr<Expr> parseExpression(const QVector<Token> &tokens);
