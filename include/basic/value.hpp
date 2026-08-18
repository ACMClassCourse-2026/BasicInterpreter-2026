#pragma once

#include <cstdint>
#include <string>
#include <variant>

namespace basic {

enum class BasicType {
    Integer,
    Long,
    Single,
    Double,
    String
};

class Value {
public:
    Value();
    explicit Value(std::int32_t value);
    explicit Value(std::int64_t value);
    explicit Value(float value);
    explicit Value(double value);
    explicit Value(std::string value);

    static Value default_value(BasicType type);

    BasicType type() const noexcept;
    bool is_numeric() const noexcept;
    std::int32_t as_integer() const;
    std::int64_t as_long() const;
    float as_single() const;
    double as_double() const;
    const std::string& as_string() const;
    Value convert_to(BasicType target) const;
    std::string format() const;

private:
    std::variant<std::int32_t, std::int64_t, float, double, std::string> data_;
};

Value add_values(const Value& left, const Value& right);
Value subtract_values(const Value& left, const Value& right);
Value multiply_values(const Value& left, const Value& right);
Value divide_values(const Value& left, const Value& right);
Value integer_divide_values(const Value& left, const Value& right);
Value modulo_values(const Value& left, const Value& right);
Value power_values(const Value& left, const Value& right);
Value negate_value(const Value& value);
int compare_values(const Value& left, const Value& right);

}  // namespace basic
