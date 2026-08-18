#include "basic/expression.hpp"

#include <cstdint>
#include <utility>

#include "basic/basic_error.hpp"
#include "basic/environment.hpp"

namespace basic {

namespace {

std::int64_t evaluate_index(const Expression& expression,
                            const Environment& environment) {
    const Value value = expression.evaluate(environment);
    if (value.type() == BasicType::Integer) {
        return value.as_integer();
    }
    if (value.type() == BasicType::Long) {
        return value.as_long();
    }
    throw BasicError(ErrorCode::TypeMismatch);
}

}  // namespace

LiteralExpression::LiteralExpression(Value value) : value_(std::move(value)) {}

Value LiteralExpression::evaluate(const Environment&) const { return value_; }

VariableExpression::VariableExpression(
    std::string name, std::optional<BasicType> suffix_type,
    std::vector<Expression*> indices)
    : name_(std::move(name)),
      suffix_type_(suffix_type),
      indices_(std::move(indices)) {}

VariableExpression::~VariableExpression() {
    for (Expression* expression : indices_) {
        delete expression;
    }
}

Value VariableExpression::evaluate(const Environment& environment) const {
    const Value* value = nullptr;
    if (indices_.empty()) {
        value = &environment.read(name_);
    } else {
        std::vector<std::int64_t> indices;
        indices.reserve(indices_.size());
        for (const Expression* expression : indices_) {
            indices.push_back(evaluate_index(*expression, environment));
        }
        value = &environment.read_element(name_, indices);
    }
    if (suffix_type_.has_value() && value->type() != suffix_type_.value()) {
        throw BasicError(ErrorCode::TypeMismatch);
    }
    return *value;
}

UnaryExpression::UnaryExpression(bool negative, Expression* operand)
    : negative_(negative), operand_(operand) {
    if (operand_ == nullptr) {
        throw std::invalid_argument("UnaryExpression requires an operand");
    }
}

UnaryExpression::~UnaryExpression() { delete operand_; }

Value UnaryExpression::evaluate(const Environment& environment) const {
    Value value = operand_->evaluate(environment);
    if (!value.is_numeric()) {
        throw BasicError(ErrorCode::TypeMismatch);
    }
    return negative_ ? negate_value(value) : value;
}

BinaryExpression::BinaryExpression(ArithmeticOperator operation,
                                   Expression* left, Expression* right)
    : operation_(operation), left_(left), right_(right) {
    if (left_ == nullptr || right_ == nullptr) {
        throw std::invalid_argument("BinaryExpression requires two operands");
    }
}

BinaryExpression::~BinaryExpression() {
    delete left_;
    delete right_;
}

Value BinaryExpression::evaluate(const Environment& environment) const {
    const Value left = left_->evaluate(environment);
    const Value right = right_->evaluate(environment);
    switch (operation_) {
        case ArithmeticOperator::Add:
            return add_values(left, right);
        case ArithmeticOperator::Subtract:
            return subtract_values(left, right);
        case ArithmeticOperator::Multiply:
            return multiply_values(left, right);
        case ArithmeticOperator::Divide:
            return divide_values(left, right);
        case ArithmeticOperator::IntegerDivide:
            return integer_divide_values(left, right);
        case ArithmeticOperator::Modulo:
            return modulo_values(left, right);
        case ArithmeticOperator::Power:
            return power_values(left, right);
    }
    throw std::logic_error("Unknown arithmetic operator");
}

ConversionExpression::ConversionExpression(ConversionKind kind,
                                           Expression* operand)
    : kind_(kind), operand_(operand) {
    if (operand_ == nullptr) {
        throw std::invalid_argument("ConversionExpression requires an operand");
    }
}

ConversionExpression::~ConversionExpression() { delete operand_; }

Value ConversionExpression::evaluate(const Environment& environment) const {
    const Value value = operand_->evaluate(environment);
    switch (kind_) {
        case ConversionKind::CInt:
            return value.convert_to(BasicType::Integer);
        case ConversionKind::CLng:
            return value.convert_to(BasicType::Long);
        case ConversionKind::CSng:
            return value.convert_to(BasicType::Single);
        case ConversionKind::CDbl:
            return value.convert_to(BasicType::Double);
    }
    throw std::logic_error("Unknown conversion kind");
}

}  // namespace basic
