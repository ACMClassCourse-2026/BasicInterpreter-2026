#pragma once

#include <stdexcept>
#include <string>

namespace basic {

enum class ErrorCode {
    Syntax,
    DivideByZero,
    InvalidNumber,
    VariableNotDefined,
    VariableAlreadyDefined,
    TypeMismatch,
    Overflow,
    InvalidArraySize,
    ArrayNotDimensioned,
    SubscriptOutOfRange,
    InvalidRedim,
    StepCannotBeZero
};

class BasicError : public std::runtime_error {
public:
    explicit BasicError(ErrorCode code);
    explicit BasicError(const std::string& message);

    ErrorCode code() const noexcept;

private:
    ErrorCode code_{ErrorCode::Syntax};
};

const char* error_message(ErrorCode code) noexcept;
[[noreturn]] void todo(const char* function_name);

}  // namespace basic
