#pragma once

#include <string>
#include <optional>
#include <vector>

#include "basic/value.hpp"

namespace basic {

class Environment;

enum class ArithmeticOperator {
    Add,
    Subtract,
    Multiply,
    Divide,
    IntegerDivide,
    Modulo,
    Power
};

enum class ConversionKind {
    CInt,
    CLng,
    CSng,
    CDbl
};

class Expression {
public:
    virtual ~Expression() = default;
    virtual Value evaluate(const Environment& environment) const = 0;
};

class LiteralExpression final : public Expression {
public:
    explicit LiteralExpression(Value value);
    Value evaluate(const Environment& environment) const override;

private:
    Value value_;
};

class VariableExpression final : public Expression {
public:
    VariableExpression(std::string name, std::optional<BasicType> suffix_type,
                       std::vector<Expression*> indices);
    ~VariableExpression() override;
    VariableExpression(const VariableExpression&) = delete;
    VariableExpression& operator=(const VariableExpression&) = delete;

    Value evaluate(const Environment& environment) const override;

private:
    std::string name_;
    std::optional<BasicType> suffix_type_;
    std::vector<Expression*> indices_;  // Owns all index expressions.
};

class UnaryExpression final : public Expression {
public:
    UnaryExpression(bool negative, Expression* operand);
    ~UnaryExpression() override;
    UnaryExpression(const UnaryExpression&) = delete;
    UnaryExpression& operator=(const UnaryExpression&) = delete;

    Value evaluate(const Environment& environment) const override;

private:
    bool negative_{false};
    Expression* operand_;  // Owns the operand.
};

class BinaryExpression final : public Expression {
public:
    BinaryExpression(ArithmeticOperator operation, Expression* left,
                     Expression* right);
    ~BinaryExpression() override;
    BinaryExpression(const BinaryExpression&) = delete;
    BinaryExpression& operator=(const BinaryExpression&) = delete;

    Value evaluate(const Environment& environment) const override;

private:
    ArithmeticOperator operation_;
    Expression* left_;   // Owns the left operand.
    Expression* right_;  // Owns the right operand.
};

class ConversionExpression final : public Expression {
public:
    ConversionExpression(ConversionKind kind, Expression* operand);
    ~ConversionExpression() override;
    ConversionExpression(const ConversionExpression&) = delete;
    ConversionExpression& operator=(const ConversionExpression&) = delete;

    Value evaluate(const Environment& environment) const override;

private:
    ConversionKind kind_;
    Expression* operand_;  // Owns the operand.
};

}  // namespace basic
