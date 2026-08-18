#pragma once

#include "basic/expression.hpp"

namespace basic {

class Environment;

enum class ComparisonOperator {
    Equal,
    NotEqual,
    Greater,
    Less,
    GreaterEqual,
    LessEqual
};

enum class LogicalOperator {
    And,
    Or
};

class Condition {
public:
    virtual ~Condition() = default;
    virtual bool evaluate(const Environment& environment) const = 0;
};

class ComparisonCondition final : public Condition {
public:
    ComparisonCondition(ComparisonOperator operation, Expression* left,
                        Expression* right);
    ~ComparisonCondition() override;
    ComparisonCondition(const ComparisonCondition&) = delete;
    ComparisonCondition& operator=(const ComparisonCondition&) = delete;

    bool evaluate(const Environment& environment) const override;

private:
    ComparisonOperator operation_;
    Expression* left_;   // Owns the left expression.
    Expression* right_;  // Owns the right expression.
};

class NotCondition final : public Condition {
public:
    explicit NotCondition(Condition* operand);
    ~NotCondition() override;
    NotCondition(const NotCondition&) = delete;
    NotCondition& operator=(const NotCondition&) = delete;

    bool evaluate(const Environment& environment) const override;

private:
    Condition* operand_;  // Owns the operand.
};

class LogicalCondition final : public Condition {
public:
    LogicalCondition(LogicalOperator operation, Condition* left,
                     Condition* right);
    ~LogicalCondition() override;
    LogicalCondition(const LogicalCondition&) = delete;
    LogicalCondition& operator=(const LogicalCondition&) = delete;

    bool evaluate(const Environment& environment) const override;

private:
    LogicalOperator operation_;
    Condition* left_;   // Owns the left condition.
    Condition* right_;  // Owns the right condition.
};

}  // namespace basic
